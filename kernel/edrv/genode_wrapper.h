#include <oplk/basictypes.h>

#ifdef __cplusplus
extern "C" {
#endif
	#include <nic_session/nic_session.h>
	#include <base/log.h>

  	void  get_Mac_Address(UINT8 addr[6]);

  	void init_Session();

#ifdef __cplusplus
} //end extern "C"
#endif