//------------------------------------------------------------------------------
// includes
//------------------------------------------------------------------------------
#include <common/oplkinc.h>
#include <common/bufalloc.h>
#include <kernel/edrv.h>

#include "genode_wrapper.h"


typedef struct
{
    tEdrvInitParam      initParam;                          ///< Init parameters
    tEdrvTxBuffer*      pTransmittedTxBufferLastEntry;      ///< Pointer to the last entry of the transmitted TX buffer
    tEdrvTxBuffer*      pTransmittedTxBufferFirstEntry;     ///< Pointer to the first entry of the transmitted Tx buffer
    void*               genodeEthThread;
} tEdrvInstance;


tOplkError edrv_init(const tEdrvInitParam* pEdrvInitParam_p);
tOplkError edrv_exit(void);
const UINT8* edrv_getMacAddr(void);
tOplkError edrv_setRxMulticastMacAddr(const UINT8* pMacAddr_p);
tOplkError edrv_clearRxMulticastMacAddr(const UINT8* pMacAddr_p);
tOplkError edrv_changeRxFilter(tEdrvFilter* pFilter_p,
                               UINT count_p,
                               UINT entryChanged_p,
                               UINT changeFlags_p);
tOplkError edrv_allocTxBuffer(tEdrvTxBuffer* pBuffer_p);
tOplkError edrv_freeTxBuffer(tEdrvTxBuffer* pBuffer_p);
tOplkError edrv_sendTxBuffer(tEdrvTxBuffer* pBuffer_p);
