#ifndef PACKETFILTER_ZEROMQ_H_
#define PACKETFILTER_ZEROMQ_H_

#include <zmq.hpp>

#include "PacketFilter.h"

class PacketFilter_ZeroMQ : public PacketFilter {
	public:
		PacketFilter_ZeroMQ(ConfigParser *cfg);
		~PacketFilter_ZeroMQ();
		void filter_packet(Packet* packet, zmq::socket_t *sock);
		void filter_setup_packet(SetupPacket* packet,bool direction);
		virtual char* toString() { return (char*)"ZeroMQ Filter"; }
};
#endif /* PACKETFILTER_ZEROMQ_H_ */
