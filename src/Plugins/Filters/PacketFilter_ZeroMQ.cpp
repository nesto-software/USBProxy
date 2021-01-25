#include "HexString.h"
#include "PacketFilter_ZeroMQ.h"
#include <zmq.hpp>
#include <msgpack.hpp>

struct ZMQ_MSG {
	std::vector<__u8> d;
	__u8 e;
	MSGPACK_DEFINE_MAP(d, e);
};

PacketFilter_ZeroMQ::PacketFilter_ZeroMQ(ConfigParser *cfg) {
    ctx = new zmq::context_t();
    sock = new zmq::socket_t(*ctx, zmq::socket_type::pub);

    (*sock).bind("tcp://127.0.0.1:5678");
}

PacketFilter_ZeroMQ::~PacketFilter_ZeroMQ() {
	(*sock).close();
	(*ctx).close();
}

void PacketFilter_ZeroMQ::filter_packet(Packet* packet) {
	if (packet->wLength<=64) {
		struct ZMQ_MSG msg;
		msg.d = std::vector<__u8>(packet->data, packet->data + packet->wLength);
		msg.e = packet->bEndpoint;

		std::stringstream buffer;
		msgpack::pack(buffer, msg);
		buffer.seekg(0);
		std::string str(buffer.str());

		(*sock).send(zmq::buffer(str), zmq::send_flags::dontwait); 
	}
}

void PacketFilter_ZeroMQ::filter_setup_packet(SetupPacket* packet,bool direction) {
	// TODO: determine if the setup packet is relevant for us
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
