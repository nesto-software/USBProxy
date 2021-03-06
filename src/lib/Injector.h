/*
 * This file is part of USBProxy.
 */

#ifndef USBPROXY_INJECTOR_H
#define USBPROXY_INJECTOR_H

#include <atomic>
#include <poll.h>

#include "Plugins.h"
#include "Packet.h"
#include "Criteria.h"
#include "ConfigParser.h"

class Injector {
private:


	int outPollIndex[16];
	int inPollIndex[16];
	std::atomic_bool _please_stop;

protected:
        PacketQueue* outQueues[16];
	PacketQueue* inQueues[16];
	//---------------------------------------------------------------------
	/// \brief gets new packets from injector pipe
	///
	/// \param[out] packet  pointer to data packet if present.  nullptr if
	///                     no data packet.
	/// \param[out] setup   pointer to setup packet if present.  nullptr if
	///                     no setup packet.
	/// \param timeout	timeout in ms to poll for new packet(s)
	///
	/// \Note	It is the responsibility of the calling routine to
	///		destroy the packet contents.
	/// \Note	This is a legacy routine to support old injectors
	//---------------------------------------------------------------------
	virtual void get_packets(Packet** packet, SetupPacket** setup, int timeout=500) {}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	virtual void start_injector() {}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	virtual void stop_injector() {}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	virtual void setup_ack() {}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	virtual void setup_stall() {}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	virtual void setup_data(__u8* buf, int length) {}
	//---------------------------------------------------------------------
	/// \brief get pollable file descriptors(s) for injection pipe
	///
	/// \return pointer to a zero terminated list of file descriptors used
	///         by get_packets();
	/// /// \Note	This is a legacy routine to support old injectors
	//---------------------------------------------------------------------
	virtual int* get_pollable_fds() {return NULL;}
	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	virtual void full_pipe(SetupPacket* p) {}
	virtual void full_pipe(Packet* p) {}

public:

	__u8 plugin_type=PLUGIN_INJECTOR;
	/// \brief endpoint filter
	struct criteria_endpoint endpoint;
	/// \brief interface filter
	struct criteria_interface interface;
	/// \brief configuration filter
	struct criteria_configuration configuration;
	/// \brief device filter
	struct criteria_device device;

	Injector();
	virtual ~Injector() {}

	//---------------------------------------------------------------------
	/// \brief used to notify the listen() task to exit
	//---------------------------------------------------------------------
	void please_stop(void) {
		_please_stop = true;
	}

	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	void set_queue(__u8 epAddress, PacketQueue& queue);

	//---------------------------------------------------------------------
	/// \brief main list loop
	///
	/// This function is  a task created by the Manager Class.
	/// The function polls for new packets to be injected from the injector
	/// pipe using get_packets.  This is for support of legacy injector
	/// Pugins.  Removing the pipes and directly injecting the data into the
	/// queue with outQueues[x].prioretyEnque dramatically (orders of
	/// magnitute) reduces the latencies in the system.
	//---------------------------------------------------------------------
	virtual void listen();

	//---------------------------------------------------------------------
	//---------------------------------------------------------------------
	virtual const char* toString() {return "Injector";}

};

extern "C" {
	Injector *get_injector_plugin();
}
#endif /* USBPROXY_INJECTOR_H */
