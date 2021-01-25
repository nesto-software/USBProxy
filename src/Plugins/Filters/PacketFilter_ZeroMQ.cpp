#include "HexString.h"
#include "PacketFilter_ZeroMQ.h"
#include <zmq.hpp>

PacketFilter_ZeroMQ::PacketFilter_ZeroMQ(ConfigParser *cfg) {
    ctx = new zmq::context_t();
    sock = new zmq::socket_t(*ctx, zmq::socket_type::push);

    (*sock).bind("tcp://127.0.0.1:5678");
}

PacketFilter_ZeroMQ::~PacketFilter_ZeroMQ() {
	// Cease any blocking operations in progress.
	(*ctx).shutdown();

	// Do a shutdown, if needed and destroy the context.
	(*ctx).close();
}

void PacketFilter_ZeroMQ::filter_packet(Packet* packet) {
	if (packet->wLength<=64) {
		char* hex=hex_string((void*)packet->data,packet->wLength);
		printf("%02x[%d]: %s\n",packet->bEndpoint,packet->wLength,hex);
		free(hex);
	}

    const std::string m = "Hello, world";
    (*sock).send(zmq::buffer(m), zmq::send_flags::dontwait); 
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
