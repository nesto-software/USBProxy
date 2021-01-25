#ifndef PACKETFILTER_ZEROMQ_H_
#define PACKETFILTER_ZEROMQ_H_

#include "PacketFilter.h"

class PacketFilter_ZeroMQ : public PacketFilter {

public:
	PacketFilter_ZeroMQ(ConfigParser *cfg);
	~PacketFilter_ZeroMQ();
	void filter_packet(Packet* packet);
	void filter_setup_packet(SetupPacket* packet,bool direction);
	virtual char* toString() {return (char*)"Stream Log Filter";}
};
#endif /* PACKETFILTER_ZEROMQ_H_ */
