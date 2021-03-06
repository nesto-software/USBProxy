/*
 * This file is part of USBProxy.
 */

#include <iostream>
#include <thread>

#include <stdio.h>
#include <sched.h>
#include <poll.h>
#include "get_tid.h"

#include "Packet.h"
#include "RelayReader.h"

#include "Endpoint.h"

#include "Proxy.h"
#include "HostProxy.h"
#include "Manager.h"

#define CTRL_REQUEST_TIMEOUT_MS 500
#define READ_TIMEOUT_MS 1500

RelayReader::RelayReader(Endpoint* _endpoint,Proxy* _proxy, PacketQueue& sendQueue, Manager* _manager)
	: _please_stop(false)
	, _sendQueue(&sendQueue)
	, _recvQueue(0)
	, manager(_manager)

{
	proxy=_proxy;
	hostProxy=NULL;
	endpoint=_endpoint->get_descriptor()->bEndpointAddress;
	attributes=_endpoint->get_descriptor()->bmAttributes;
	maxPacketSize=_endpoint->get_descriptor()->wMaxPacketSize;
}

RelayReader::RelayReader(Endpoint* _endpoint,HostProxy* _hostProxy, PacketQueue& sendQueue, PacketQueue& recvQueue, Manager* _manager)
	: _please_stop(false)
	, _sendQueue(&sendQueue)
	, _recvQueue(&recvQueue)
	, manager(_manager)
{
	proxy=NULL;
	hostProxy=_hostProxy;
	endpoint=_endpoint->get_descriptor()->bEndpointAddress;
	attributes=_endpoint->get_descriptor()->bmAttributes;
	maxPacketSize=_endpoint->get_descriptor()->wMaxPacketSize;

}

RelayReader::~RelayReader() {
}

void RelayReader::relay_read_setup() {
	if (!hostProxy) {fprintf(stderr,"HostProxy not initialized for EP00 reader.\n");return;}
	if (!_recvQueue) {fprintf(stderr,"inQueue not initialized for EP00 reader.\n");return;}

	bool idle=true;
	__u8* buf;
	int length;
	PacketPtr p;

	bool direction_out=true;
	usb_ctrlrequest ctrl_req;

	fprintf(stderr,"Starting setup reader thread (%ld) for EP%02x.\n",gettid(),endpoint);
	while (!_please_stop) {
		idle=true;
		if (direction_out) {
			buf=NULL;
			length=0;

			int rc = hostProxy->control_request(&ctrl_req, &length, &buf, CTRL_REQUEST_TIMEOUT_MS);
			if (_please_stop)
				break;
			switch (rc)
			{
			case CONTROL_REQUEST_SETUP:
				p = std::make_shared<SetupPacket>(ctrl_req,buf);
				if (!p)
					continue;
				_sendQueue->enqueue(p);
				direction_out=false;
				p.reset();
				idle=false;
				break;

			case CONTROL_REQUEST_DISCONNECT:
				manager->hostDisconnectNotification();
				break;

			case CONTROL_REQUEST_CONNECT:
				manager->hostConnectNotification();
				break;

 			default:
				continue;
			}
		} else {
			if (!p) {
				p = _recvQueue->dequeue();
				if (_please_stop)
					break;
				if (!p)
					continue;
				SetupPacket* s = dynamic_cast<SetupPacket*>(p.get());
				if (!s) {
					p.reset();
					continue;
				}
				if (s->transmit_in) {
					if (s->ctrl_req.wLength) {
						hostProxy->send_data(endpoint,attributes,maxPacketSize,s->data,s->ctrl_req.wLength);
					} else {
						hostProxy->control_ack();
					}
				} else {
					hostProxy->stall_ep(endpoint);
				}
			}
			if (_please_stop)
					break;
			if (p) {
				if (hostProxy->send_wait_complete(endpoint, CTRL_REQUEST_TIMEOUT_MS)) {
					direction_out=true;
					p.reset();
					idle=false;
				}
			}
		}
		if (idle) sched_yield();
	}
	fprintf(stderr,"Finished setup reader thread (%ld) for EP%02x.\n",gettid(),endpoint);
	_please_stop = false;
}

void RelayReader::relay_read() {
	if (!endpoint) {
		relay_read_setup();
		return;
	}

	__u8* buf;
	int length;
	unsigned zlpCount = 0;
	fprintf(stderr,"Starting reader thread (%ld) for EP%02x.\n",gettid(),endpoint);
	while (!_please_stop) {
		buf = nullptr;
		length = 0;

		proxy->receive_data(endpoint,attributes,maxPacketSize,&buf,&length, READ_TIMEOUT_MS);
		if (_please_stop)
		{
			free(buf);
			break;
		}
		if(length)  {
			_sendQueue->enqueue(std::make_shared<Packet>(endpoint, buf, length));
			zlpCount = 0;
		}
		// Receive data can return two different results for a length of
		// zero.  The two are differentiated by the value of buf.  If
		// buf == nullptr there was no data received.  If buf != nullptr
		// a ZLP was received.
		//
		// Zero Lenth Packets (ZLP) can be a problem.  If all ZLPs are
		// passed the performance will suffer and buffers may
		// overflow.  ZLP cannot be completely blocked as they are
		// required in some cases by the usb protocols (i.e. used for
		// bulk transfers that are less than the requested size and a
		// multiple of the maximum packet size).  I have also found that
		// some drivers expect ZLP for no data and will have problems
		// if they just get NAKs (gadgetfs default way to present no
		// data).
		//
		// Two methods are used to reduce ZLPs.  relay_read only sends a
		// ZLP after a non ZLP, or when the sendQuee is empty.
		// The flow of ZLP can is also be limited  with
		// DeviceProxy::nice.  DeviceProxy::nice reduces the device
		// polling rate which  also reduces the ZLP rate.
		//
		// todo: review: this is only really needed  when reading from
		// the deviceProxy.  Should we change/disable for hostProxy?
		else if (nullptr != buf) {

			if ((!zlpCount) || _sendQueue->empty() ) {
				_sendQueue->enqueue(std::make_shared<Packet>(endpoint, buf, length));
				++zlpCount;
			} else {
			        ++zlpCount;
				free(buf);
			}
		}
	}
	fprintf(stderr,"Finished reader thread (%ld) for EP%02x.\n",gettid(),endpoint);
	_please_stop = false;
}
