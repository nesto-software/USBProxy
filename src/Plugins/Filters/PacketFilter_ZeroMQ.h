#ifndef PACKETFILTER_ZEROMQ_H_
#define PACKETFILTER_ZEROMQ_H_

#include "PacketFilter.h"
#include <zmq.hpp>

class PacketFilter_ZeroMQ : public PacketFilter {
	private:
    	zmq::socket_t *sock;

	public:
		PacketFilter_ZeroMQ(ConfigParser *cfg);
		~PacketFilter_ZeroMQ();
		void filter_packet(Packet* packet);
		void filter_setup_packet(SetupPacket* packet,bool direction);
		virtual char* toString() { return (char*)"ZeroMQ Filter"; }
};
#endif /* PACKETFILTER_ZEROMQ_H_ */
