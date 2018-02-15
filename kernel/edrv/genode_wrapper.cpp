#include "genode_wrapper.h"

#ifdef __cplusplus

#include <base/thread.h>
#include <base/log.h>
#include <nic/packet_allocator.h>
#include <nic_session/connection.h>


PACKET_SIZE 	1536;

extern "C" {
#endif
	
	using namespace Genode;
	using namespace Net;

	Nic::Connection  *_nic;       /* nic-session */

  	Nic::Connection  *nic() { return _nic; };

	int init_Session() {
	    Genode::log("Open NIC Session");
		/* Initialize nic-session */
		Nic::Packet_allocator *tx_block_alloc = new (env()->heap())
		                                        Nic::Packet_allocator(env()->heap());

		int buf_size    = Nic::Session::QUEUE_SIZE * PACKET_SIZE;

		try {
			_nic = new (env()->heap()) Nic::Connection(tx_block_alloc,
			                                          buf_size,
			                                          buf_size);
		} catch (Parent::Service_denied) {
			destroy(env()->heap(), tx_block_alloc);
			return 1;
		}

		return 0;
  	}

  	void get_Mac_Address(UINT8 addr[6]) {
	    Genode::log("Getting MAC Address");

    	//Get Mac Address from Genode NIC
    	Nic::Mac_address _mac_address = nic()->mac_address();

    	for(int i=0; i<6; ++i)
			addr[i] = _mac_address.addr[i];
  	}

  	void _tx_ack(bool block) {
		/* check for acknowledgements */
		while (nic()->tx()->ack_avail() || block) {
			Nic::Packet_descriptor acked_packet = nic()->tx()->get_acked_packet();
			nic()->tx()->release_packet(acked_packet);
			block = false;
		}
  	}


  	void sendTXBuffer(unsigned char* buffer, size_t size) {
  		Nic::Packet_descriptor packet;
  		bool end = FALSE;

  		while (!end) {
            try {
                packet = nic()->tx()->alloc_packet(size);
                end = TRUE;
            } catch(Nic::Session::Tx::Source::Packet_alloc_failed) {
                /* packet allocator exhausted, wait for acknowledgements */
                _tx_ack(true);
            }
        }

        char *tx_content = nic()->tx()->packet_content(packet);

		/*
		 * copy payload into packet's payload
		 */
		Genode::memcpy(tx_content, buffer, size);

		/* Submit packet */
		nic()->tx()->submit_packet(packet);
		/* check for acknowledgements */
		_tx_ack(false);
    }
    
#ifdef __cplusplus
} //end extern "C"
#endif
