#include <oplk/basictypes.h>
#include <common/oplkinc.h>

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

  	void  get_Mac_Address(UINT8 addr[6]);

  	int init_Session(struct tEdrvInstance* init);

  	void sendTXBuffer(struct tEdrvInstance* init, unsigned char* buffer, size_t size);

	void _tx_ack(bool block);

#ifdef __cplusplus
} //end extern "C"
#endif