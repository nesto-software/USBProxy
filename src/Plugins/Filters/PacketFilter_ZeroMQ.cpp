#include <zmq.hpp>
#include <msgpack.hpp>

#include "HexString.h"
#include "PacketFilter_ZeroMQ.h"

struct ZMQ_MSG {
	std::vector<__u8> d;
	__u8 e;
	MSGPACK_DEFINE_MAP(d, e);
};

PacketFilter_ZeroMQ::PacketFilter_ZeroMQ(ConfigParser *cfg) {}
PacketFilter_ZeroMQ::~PacketFilter_ZeroMQ() {}

void PacketFilter_ZeroMQ::filter_packet(Packet* packet, zmq::socket_t *sock) {
	struct ZMQ_MSG msg;
	msg.d = std::vector<__u8>(packet->data, packet->data + packet->wLength);
	msg.e = packet->bEndpoint;

	std::stringstream buffer;
	msgpack::pack(buffer, msg);
	buffer.seekg(0);
	std::string str(buffer.str());

	(*sock).send(zmq::buffer(str), zmq::send_flags::dontwait); 

	// TODO: is there something to clean up / free?
}

void PacketFilter_ZeroMQ::filter_setup_packet(SetupPacket* packet,bool direction) {}

static PacketFilter_ZeroMQ *proxy;

extern "C" {
	int plugin_type = PLUGIN_FILTER;
	
	PacketFilter * get_plugin(ConfigParser *cfg) {
		proxy = new PacketFilter_ZeroMQ(cfg);
		return (PacketFilter *) proxy;
	}
	
	void destroy_plugin() {
		delete proxy;
	}
}
