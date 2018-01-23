#include "genode_wrapper.h"

extern "C" {

	#include <nic_session/nic_session.h>

	#include <base/log.h>


  	void get_Mac_Address(UINT8 addr[6]) {
	    Genode::log("Getting MAC Address");
    	//Get Mac Address from Genode NIC
    	struct Nic::Session nic;
    	Net::Mac_address mac_address;

    	mac_address = nic.mac_address();

	    mac_address.copy(addr);
  	}

	void init_Session() {

  	}
}