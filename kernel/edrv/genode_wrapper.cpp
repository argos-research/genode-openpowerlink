//C++-source: sys_unicode_for_c.hpp
#include "genode_wrapper.h"

extern "C" {
  void get_Mac_Address(uint8_t addr[6]) {
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