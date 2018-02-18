#include <oplk/oplk.h>
#include <oplk/debugstr.h>

#include <system/system.h>
#include <obdcreate/obdcreate.h>

#include <stdio.h>
#include <limits.h>

#include "genode_functions.h"
#include "event.h"
#include "app.h"

#define CYCLE_LEN           50000
#define NODEID              1
#define IP_ADDR             0xc0a86401          // 192.168.100.1
#define DEFAULT_GATEWAY     0xC0A864FE          // 192.168.100.C_ADR_RT1_DEF_NODE_ID
#define SUBNET_MASK         0xFFFFFF00          // 255.255.255.0

static const UINT8  aMacAddr_l[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static tOplkError initPowerlink(UINT32 cycleLen_p,
                                const char* devName_p,
                                const UINT8* macAddr_p,
                                UINT32 nodeId_p);

static void loopMain(void);

int main()
{
	tOplkError  ret = kErrorOk;
	printConsole("Start loading RT Ethernet Stack...\n");
	ret = initPowerlink(CYCLE_LEN,
                        "\0",
                        aMacAddr_l,
                        NODEID);
	printConsole("Finished loading RT Ethernet Stack!\n");

	printConsole("Start loading App...\n");
	ret = initApp();
	printConsole("Finished loading App!\n");

	loopMain();

	printConsole("Return Code was: ");

	return ret;
}

static tOplkError initPowerlink(UINT32 cycleLen_p,
                                const char* devName_p,
                                const UINT8* macAddr_p,
                                UINT32 nodeId_p)
{
    tOplkError          ret = kErrorOk;
    tOplkApiInitParam   initParam;
    static char         devName[128];

    printConsole("Initializing openPOWERLINK stack...\n");

    UNUSED_PARAMETER(devName_p);

    memset(&initParam, 0, sizeof(initParam));
    initParam.sizeOfInitParam = sizeof(initParam);

    // pass selected device name to Edrv
    initParam.hwParam.pDevName = devName;
    initParam.nodeId = nodeId_p;
    initParam.ipAddress = (0xFFFFFF00 & IP_ADDR) | initParam.nodeId;

    /* write 00:00:00:00:00:00 to MAC address, so that the driver uses the real hardware address */
    memcpy(initParam.aMacAddress, macAddr_p, sizeof(initParam.aMacAddress));

    initParam.fAsyncOnly              = FALSE;
    initParam.featureFlags            = UINT_MAX;
    initParam.cycleLen                = cycleLen_p;             // required for error detection
    initParam.isochrTxMaxPayload      = C_DLL_ISOCHR_MAX_PAYL;  // const
    initParam.isochrRxMaxPayload      = C_DLL_ISOCHR_MAX_PAYL;  // const
    initParam.presMaxLatency          = 50000;                  // const; only required for IdentRes
    initParam.preqActPayloadLimit     = 36;                     // required for initialization (+28 bytes)
    initParam.presActPayloadLimit     = 36;                     // required for initialization of Pres frame (+28 bytes)
    initParam.asndMaxLatency          = 150000;                 // const; only required for IdentRes
    initParam.multiplCylceCnt         = 0;                      // required for error detection
    initParam.asyncMtu                = 1500;                   // required to set up max frame size
    initParam.prescaler               = 2;                      // required for sync
    initParam.lossOfFrameTolerance    = 500000;
    initParam.asyncSlotTimeout        = 3000000;
    initParam.waitSocPreq             = 1000;
    initParam.deviceType              = UINT_MAX;               // NMT_DeviceType_U32
    initParam.vendorId                = UINT_MAX;               // NMT_IdentityObject_REC.VendorId_U32
    initParam.productCode             = UINT_MAX;               // NMT_IdentityObject_REC.ProductCode_U32
    initParam.revisionNumber          = UINT_MAX;               // NMT_IdentityObject_REC.RevisionNo_U32
    initParam.serialNumber            = UINT_MAX;               // NMT_IdentityObject_REC.SerialNo_U32
    initParam.applicationSwDate       = 0;
    initParam.applicationSwTime       = 0;
    initParam.subnetMask              = SUBNET_MASK;
    initParam.defaultGateway          = DEFAULT_GATEWAY;
    sprintf((char*)initParam.sHostname, "%02x-%08x", initParam.nodeId, initParam.vendorId);
    initParam.syncNodeId              = C_ADR_SYNC_ON_SOA;
    initParam.fSyncOnPrcNode          = FALSE;

    // set callback functions
    initParam.pfnCbEvent = processEvents;

#if defined(CONFIG_KERNELSTACK_DIRECTLINK)
//    initParam.pfnCbSync = processSync;
#else
    initParam.pfnCbSync = NULL;
#endif

    printConsole("Start with obdcreate_initObd");
    // Initialize object dictionary
    ret = obdcreate_initObd(&initParam.obdInitParam);
    if(&initParam.obdInitParam==NULL)
	printConsole("pInitParam_p->pGenericPart == NULL after Init");
    if (ret != kErrorOk)
    {
        printConsole("obdcreate_initObd() failed\n");//,
//                debugstr_getRetValStr(ret),
//                ret);
        return ret;
    }
    printConsole("Success");

    // initialize POWERLINK stack
    printConsole("Start with oplk_initialize");
    ret = oplk_initialize();
    if (ret != kErrorOk)
    {
        printConsole("oplk_initialize() failed\n");//,
//                debugstr_getRetValStr(ret),
//                ret);
        return ret;
    }
    printConsole("Success");

    printConsole("Start with oplk_create");
    ret = oplk_create(&initParam);
    if (ret != kErrorOk)
    {
        printConsole("oplk_create() failed\n");//,
//                debugstr_getRetValStr(ret),
//                ret);
        return ret;
    }
    printConsole("Success");

    return kErrorOk;
}










static void loopMain(void)
{
    tOplkError  ret;
    char        cKey = 0;
    BOOL        fExit = FALSE;

#if !defined(CONFIG_KERNELSTACK_DIRECTLINK)

#if defined(CONFIG_USE_SYNCTHREAD)
    system_startSyncThread(processSync);
#endif

#endif

    // start processing
    ret = oplk_execNmtCommand(kNmtEventSwReset);
    if (ret != kErrorOk)
        return;

    printConsole("Start POWERLINK stack... ok\n");
    printConsole("Digital I/O interface with openPOWERLINK is ready!\n");
    printConsole("\n-------------------------------\n");
    printConsole("Press Esc to leave the program\n");
    printConsole("Press r to reset the node\n");
    printConsole("Press i to increase the digital input\n");
    printConsole("Press d to decrease the digital input\n");
    printConsole("Press p to print the digital outputs\n");
    printConsole("-------------------------------\n\n");

    setupInputs();

    // wait for key hit
    while (!fExit)
    {
        /*if (console_kbhit())
        {
            cKey = (char)console_getch();

            switch (cKey)
            {
                case 'r':
                    ret = oplk_execNmtCommand(kNmtEventSwReset);
                    if (ret != kErrorOk)
                        fExit = TRUE;
                    break;

                case 'i':
                    increaseInputs();
                    break;

                case 'd':
                    decreaseInputs();
                    break;

                case 'p':
                    printOutputs();
                    break;

                case 0x1B:
                    fExit = TRUE;
                    break;

                default:
                    break;
            }
        }*/

        if (system_getTermSignalState() == TRUE)
        {
            fExit = TRUE;
            printConsole("Received termination signal, exiting...\n");
        }

        if (oplk_checkKernelStack() == FALSE)
        {
            fExit = TRUE;
            printConsole("Kernel stack has gone! Exiting...\n");
        }

#if (defined(CONFIG_USE_SYNCTHREAD) || \
     defined(CONFIG_KERNELSTACK_DIRECTLINK))
        system_msleep(100);
#else
        processSync();
#endif
    }
}
