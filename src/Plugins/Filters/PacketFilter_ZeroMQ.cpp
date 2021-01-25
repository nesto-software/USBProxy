#include "HexString.h"
#include "PacketFilter_ZeroMQ.h"
#include <zmq.hpp>

PacketFilter_ZeroMQ::PacketFilter_ZeroMQ(ConfigParser *cfg) {
    zmq::context_t ctx;
    zmq::socket_t sock(ctx, zmq::socket_type::push);
    sock.bind("inproc://test");
    const std::string_view m = "Hello, world";
    sock.send(zmq::buffer(m), zmq::send_flags::dontwait);
}

PacketFilter_ZeroMQ::~PacketFilter_ZeroMQ() {
	// TODO: cleanup
}

void PacketFilter_ZeroMQ::filter_packet(Packet* packet) {
	if (packet->wLength<=64) {
		char* hex=hex_string((void*)packet->data,packet->wLength);
		printf("%02x[%d]: %s\n",packet->bEndpoint,packet->wLength,hex);
		free(hex);
	}
}

void PacketFilter_ZeroMQ::filter_setup_packet(SetupPacket* packet,bool direction) {

}

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
