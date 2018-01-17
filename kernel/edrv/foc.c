//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <common/oplkinc.h>
#include <common/bufalloc.h>
#include <kernel/edrv.h>

//############################################################################################ #include <base/printf.h>
//############################################################################################ //#include <os/attached_ram_dataspace.h>
//############################################################################################ #include <nic/packet_allocator.h>
//############################################################################################ #include <net/ethernet.h>


//============================================================================//
//            G L O B A L   D E F I N I T I O N S                             //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// module global vars
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global function prototypes
//------------------------------------------------------------------------------

//============================================================================//
//            P R I V A T E   D E F I N I T I O N S                           //
//============================================================================//

//------------------------------------------------------------------------------
// const defines
//------------------------------------------------------------------------------
#define EDRV_MAX_FRAME_SIZE     0x0600

//############################################################################################using namespace Net;
//------------------------------------------------------------------------------
// local types
//------------------------------------------------------------------------------
/**
\brief Structure describing an instance of the Edrv

This structure describes an instance of the Ethernet driver.
*/
typedef struct
{
    tEdrvInitParam      initParam;                          ///< Init parameters
    tEdrvTxBuffer*      pTransmittedTxBufferLastEntry;      ///< Pointer to the last entry of the transmitted TX buffer
    tEdrvTxBuffer*      pTransmittedTxBufferFirstEntry;     ///< Pointer to the first entry of the transmitted Tx buffer
} tEdrvInstance;

//------------------------------------------------------------------------------
// local vars
//------------------------------------------------------------------------------
static tEdrvInstance edrvInstance_l;

//============================================================================//
//            P U B L I C   F U N C T I O N S                                 //
//============================================================================//

//------------------------------------------------------------------------------
/**
\brief  Ethernet driver initialization

This function initializes the Ethernet driver.

\param[in]      pEdrvInitParam_p    Edrv initialization parameters

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_init(const tEdrvInitParam* pEdrvInitParam_p)
{
    int         i;
    
    // Check parameter validity
    ASSERT(pEdrvInitParam_p != NULL);

    // clear instance structure
    OPLK_MEMSET(&edrvInstance_l, 0, sizeof(edrvInstance_l));

    // save the init data
    edrvInstance_l.initParam = *pEdrvInitParam_p;


    // read MAC address from controller
    printk("%s local MAC = ", __FUNCTION__);
    for (i = 0; i < 6; i++)
    {
        edrvInstance_l.initParam.aMacAddr[i] = 0xFF;//############################################################################################Net::mac()[i];
        printk("%02X ", (UINT)edrvInstance_l.initParam.aMacAddr[i]);
    }
    printk("\n");

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Shut down Ethernet driver

This function shuts down the Ethernet driver.

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_exit(void)
{
    /*if (edrvDriver_l.name != NULL)
    {
        // clear buffer allocation
        bufalloc_exit(pBufAlloc_l);
        pBufAlloc_l = NULL;
        // clear driver structure
        OPLK_MEMSET(&edrvDriver_l, 0, sizeof(edrvDriver_l));
    }*/

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Get MAC address

This function returns the MAC address of the Ethernet controller

\return The function returns a pointer to the MAC address.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
const UINT8* edrv_getMacAddr(void)
{
    return edrvInstance_l.initParam.aMacAddr;
}

//------------------------------------------------------------------------------
/**
\brief  Set multicast address entry

This function sets a multicast entry into the Ethernet controller.

\param[in]      pMacAddr_p          Multicast address.

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_setRxMulticastMacAddr(const UINT8* pMacAddr_p)
{
    UNUSED_PARAMETER(pMacAddr_p);

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Clear multicast address entry

This function removes the multicast entry from the Ethernet controller.

\param[in]      pMacAddr_p          Multicast address

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_clearRxMulticastMacAddr(const UINT8* pMacAddr_p)
{
    UNUSED_PARAMETER(pMacAddr_p);

    return kErrorOk;
}
//------------------------------------------------------------------------------
/**
\brief  Change Rx filter setup

This function changes the Rx filter setup. The parameter entryChanged_p
selects the Rx filter entry that shall be changed and \p changeFlags_p determines
the property.
If \p entryChanged_p is equal or larger count_p all Rx filters shall be changed.

\note Rx filters are not supported by this driver!

\param[in,out]  pFilter_p           Base pointer of Rx filter array
\param[in]      count_p             Number of Rx filter array entries
\param[in]      entryChanged_p      Index of Rx filter entry that shall be changed
\param[in]      changeFlags_p       Bit mask that selects the changing Rx filter property

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_changeRxFilter(tEdrvFilter* pFilter_p,
                               UINT count_p,
                               UINT entryChanged_p,
                               UINT changeFlags_p)
{
    UNUSED_PARAMETER(pFilter_p);
    UNUSED_PARAMETER(count_p);
    UNUSED_PARAMETER(entryChanged_p);
    UNUSED_PARAMETER(changeFlags_p);

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Allocate Tx buffer

This function allocates a Tx buffer.

\param[in,out]  pBuffer_p           Tx buffer descriptor

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_allocTxBuffer(tEdrvTxBuffer* pBuffer_p)
{
    // Check parameter validity
    ASSERT(pBuffer_p != NULL);

    if (pBuffer_p->maxBufferSize > EDRV_MAX_FRAME_SIZE)
        return kErrorEdrvNoFreeBufEntry;

    // allocate buffer with malloc
    pBuffer_p->pBuffer = (UINT8*)OPLK_MALLOC(pBuffer_p->maxBufferSize);
    if (pBuffer_p->pBuffer == NULL)
        return kErrorEdrvNoFreeBufEntry;

    pBuffer_p->txBufferNumber.pArg = NULL;

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Free Tx buffer

This function releases the Tx buffer.

\param[in,out]  pBuffer_p           Tx buffer descriptor

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_freeTxBuffer(tEdrvTxBuffer* pBuffer_p)
{
    UINT8* pBuffer;

    // Check parameter validity
    ASSERT(pBuffer_p != NULL);

    pBuffer = pBuffer_p->pBuffer;

    // mark buffer as free, before actually freeing it
    pBuffer_p->pBuffer = NULL;

    OPLK_FREE(pBuffer);

    return kErrorOk;
}

//------------------------------------------------------------------------------
/**
\brief  Send Tx buffer

This function sends the Tx buffer.

\param[in,out]  pBuffer_p           Tx buffer descriptor

\return The function returns a tOplkError error code.

\ingroup module_edrv
*/
//------------------------------------------------------------------------------
tOplkError edrv_sendTxBuffer(tEdrvTxBuffer* pBuffer_p)
{
    /*tOplkError  ret = kErrorOk;
    UINT        bufferNumber;
    UINT32      temp;
    ULONG       flags;

    // Check parameter validity
    ASSERT(pBuffer_p != NULL);

    bufferNumber = pBuffer_p->txBufferNumber.value;

    if (pBuffer_p->pBuffer == NULL)
    {
        ret = kErrorEdrvBufNotExisting;
        goto Exit;
    }

    if ((bufferNumber >= EDRV_MAX_TX_BUFFERS) ||
        (edrvInstance_l.afTxBufUsed[bufferNumber] == FALSE))
    {
        ret = kErrorEdrvBufNotExisting;
        goto Exit;
    }

    // array of pointers to tx buffers in queue is checked
    // because all four tx descriptors should be used
    if (edrvInstance_l.apTxBuffer[edrvInstance_l.tailTxDesc] != NULL)
    {
        ret = kErrorEdrvNoFreeTxDesc;
        goto Exit;
    }

    EDRV_COUNT_SEND;

    // pad with zeros if necessary, because controller does not do it
    if (pBuffer_p->txFrameSize < EDRV_MIN_ETH_SIZE)
    {
        OPLK_MEMSET(pBuffer_p->pBuffer + pBuffer_p->txFrameSize, 0, EDRV_MIN_ETH_SIZE - pBuffer_p->txFrameSize);
        pBuffer_p->txFrameSize = EDRV_MIN_ETH_SIZE;
    }

    spin_lock_irqsave(&edrvInstance_l.txSpinlock, flags);

    // save pointer to buffer structure for TxHandler
    edrvInstance_l.apTxBuffer[edrvInstance_l.tailTxDesc] = pBuffer_p;

    // set DMA address of buffer
    EDRV_REGDW_WRITE(EDRV_REGDW_TSAD(edrvInstance_l.tailTxDesc), (edrvInstance_l.pTxBufDma + (bufferNumber * EDRV_MAX_FRAME_SIZE)));
    temp = EDRV_REGDW_READ(EDRV_REGDW_TSAD(edrvInstance_l.tailTxDesc));

    // start transmission
    EDRV_REGDW_WRITE(EDRV_REGDW_TSD(edrvInstance_l.tailTxDesc), (EDRV_REGDW_TSD_TXTH_DEF | pBuffer_p->txFrameSize));
    temp = EDRV_REGDW_READ(EDRV_REGDW_TSD(edrvInstance_l.tailTxDesc));

    // increment tx queue tail
    edrvInstance_l.tailTxDesc = (edrvInstance_l.tailTxDesc + 1) & EDRV_TX_DESC_MASK;

    spin_unlock_irqrestore(&edrvInstance_l.txSpinlock, flags);

Exit:
    return ret;*/
    return kErrorOk;
}



/// \}
