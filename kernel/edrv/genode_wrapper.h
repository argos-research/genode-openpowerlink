//C/C++ - header "sys_unicode_for_c.h"

#ifdef __cplusplus
extern "C" {
#endif

  #include <nic_session/nic_session.h>

  #include <base/log.h>

  void  get_Mac_Address(uint8_t addr[6]);

  void init_Session();

#ifdef __cplusplus
} //end extern "C"
#endif