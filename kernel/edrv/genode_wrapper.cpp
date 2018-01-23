#include "genode_wrapper.h"

#ifdef __cplusplus

#include <nic_session/nic_session.h>
#include <base/log.h>


extern "C" {
#endif
	
	using namespace Genode;
	using namespace Nic;
	using namespace Net;

  	void get_Mac_Address(UINT8 addr[6]) {
	    Genode::log("Getting MAC Address");
    	//Get Mac Address from Genode NIC
    	struct Nic nic;
    	Net::Mac_address mac_address;

    	mac_address = nic.mac_address();

	    mac_address.copy(addr);
  	}

	void init_Session() {

  	}
#ifdef __cplusplus
} //end extern "C"
#endif