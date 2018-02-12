#include <oplk/basictypes.h>

#ifdef __cplusplus
extern "C" {
#endif

public:
	Nic::Connection  *_nic;       /* nic-session */

  	void  get_Mac_Address(UINT8 addr[6]);

  	int init_Session();

  	void sendTXBuffer(char* buffer, size_t size);

  	Nic::Connection  *nic() { return _nic; };

private:
	void _tx_ack(bool block = false);

#ifdef __cplusplus
} //end extern "C"
#endif