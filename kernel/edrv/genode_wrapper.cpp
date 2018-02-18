#include "genode_wrapper.h"

#ifdef __cplusplus

#include <base/thread.h>
#include <base/log.h>
#include <nic/packet_allocator.h>
#include <nic_session/connection.h>

#define PACKET_SIZE 	1536;

    /*
 * Thread, that receives packets by the nic-session interface.
 */
class Nic_receiver_thread : public Genode::Thread_deprecated<8192>
{
	private:

		typedef Nic::Packet_descriptor Packet_descriptor;

		Nic::Connection  *_nic;       /* nic-session */
		Packet_descriptor _rx_packet; /* actual packet received */
		tEdrvInstance *_init;

		Genode::Signal_receiver  _sig_rec;

		Genode::Signal_dispatcher<Nic_receiver_thread> _rx_packet_avail_dispatcher;
		Genode::Signal_dispatcher<Nic_receiver_thread> _rx_ready_to_ack_dispatcher;

		void _handle_rx_packet_avail(unsigned)
		{
			while (_nic->rx()->packet_avail() && _nic->rx()->ready_to_ack()) {
				_rx_packet = _nic->rx()->get_packet();
				process_input(_init);
				_nic->rx()->acknowledge_packet(_rx_packet);
			}
		}

		void _handle_rx_read_to_ack(unsigned) { _handle_rx_packet_avail(0); }

		void _tx_ack(bool block = false)
		{
			/* check for acknowledgements */
			while (nic()->tx()->ack_avail() || block) {
				Packet_descriptor acked_packet = nic()->tx()->get_acked_packet();
				nic()->tx()->release_packet(acked_packet);
				block = false;
			}
		}

	public:

		Nic_receiver_thread(Nic::Connection *nic, tEdrvInstance *init)
		:
			Genode::Thread_deprecated<8192>("nic-recv"), _nic(nic), _init(init),
			_rx_packet_avail_dispatcher(_sig_rec, *this, &Nic_receiver_thread::_handle_rx_packet_avail),
			_rx_ready_to_ack_dispatcher(_sig_rec, *this, &Nic_receiver_thread::_handle_rx_read_to_ack)
		{
			_nic->rx_channel()->sigh_packet_avail(_rx_packet_avail_dispatcher);
			_nic->rx_channel()->sigh_ready_to_ack(_rx_ready_to_ack_dispatcher);
		}

		void entry();
		Nic::Connection  *nic() { return _nic; };
		Packet_descriptor rx_packet() { return _rx_packet; };

		void sendTXBufferWorkerThread(unsigned char* buffer, size_t size) {
			Genode::log("NIC Thread sendTXBufferWorkerThread()");
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

	    	static void	process_input(tEdrvInstance *init)
		{
			Genode::log("NIC Thread process_input()");

			Nic_receiver_thread   *th         = reinterpret_cast<Nic_receiver_thread*>(init->genodeEthThread);
			Nic::Connection       *nic        = th->nic();
			Nic::Packet_descriptor rx_packet  = th->rx_packet();
			char                  *rx_content = nic->rx()->packet_content(rx_packet);
			//u16_t                  len        = rx_packet.size();

			Genode::log((char const *)rx_content);
			/* We allocate a pbuf chain of pbufs from the pool. */
	//		struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);

				/*
				 * We iterate over the pbuf chain until we have read the entire
				 * packet into the pbuf.
				 */
	/*			for(struct pbuf *q = p; q != 0; q = q->next) {
					char *dst = (char*)q->payload;
					Genode::memcpy(dst, rx_content, q->len);
					rx_content += q->len;
				}
	*/
			//return p;
		}
};

/*
 * C-interface
 */
extern "C" {
#endif
	using namespace Genode;
	using namespace Net;

	Nic::Connection  *_nic;       /* nic-session */

  	Nic::Connection  *nic() { return _nic; };

	int init_Session(tEdrvInstance *init) {
	    Genode::log("Open NIC Session");
		/* Initialize nic-session */
		Nic::Packet_allocator *tx_block_alloc = new (env()->heap())Nic::Packet_allocator(env()->heap());

		int buf_size    = Nic::Session::QUEUE_SIZE * PACKET_SIZE;

		try {
			_nic = new (env()->heap()) Nic::Connection(tx_block_alloc,
			                                          buf_size,
			                                          buf_size);
		} catch (Parent::Service_denied) {
			destroy(env()->heap(), tx_block_alloc);
			return 1;
		}

		Nic_receiver_thread *th = new (env()->heap())Nic_receiver_thread(nic(), init);

		init->genodeEthThread = (void*) th;

		th->start();

		return 0;
  	}

  	void get_Mac_Address(UINT8 addr[6]) {
	    Genode::log("Getting MAC Address");

    	//Get Mac Address from Genode NIC
    	Nic::Mac_address _mac_address = nic()->mac_address();

    	for(int i=0; i<6; ++i)
			addr[i] = _mac_address.addr[i];

		Genode::log(_mac_address);
  	}


  	void sendTXBuffer(tEdrvInstance *init, unsigned char* buffer, size_t size) {
  		Nic_receiver_thread *th = reinterpret_cast<Nic_receiver_thread*>(init->genodeEthThread);

		th->sendTXBufferWorkerThread(buffer, size);
    }
    




#ifdef __cplusplus
} //end extern "C"
#endif

void Nic_receiver_thread::entry()
		{
			Genode::log("NIC Thread entry() started");
			while(true)
			{
				Genode::Signal sig = _sig_rec.wait_for_signal();
				int num    = sig.num();

				Genode::Signal_dispatcher_base *dispatcher;
				dispatcher = dynamic_cast<Genode::Signal_dispatcher_base *>(sig.context());
				dispatcher->dispatch(num);
			}
		}
