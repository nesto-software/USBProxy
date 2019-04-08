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

RelayReader::RelayReader(Endpoint* _endpoint,Proxy* _proxy, PacketQueue& sendQueue, Manager* _manager, unsigned nice_)
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
	setNice(nice_);
}

RelayReader::RelayReader(Endpoint* _endpoint,HostProxy* _hostProxy, PacketQueue& sendQueue, PacketQueue& recvQueue, Manager* _manager)
	: _please_stop(false)
	, _sendQueue(&sendQueue)
	, _recvQueue(&recvQueue)
	, manager(_manager)
	, nice(1)

{
	proxy=NULL;
	hostProxy=_hostProxy;
	endpoint=_endpoint->get_descriptor()->bEndpointAddress;
	attributes=_endpoint->get_descriptor()->bmAttributes;
	maxPacketSize=_endpoint->get_descriptor()->wMaxPacketSize;

}

RelayReader::~RelayReader() {
}

void RelayReader::setNice(unsigned nice)
{
	nice = (0 == nice)? 1:nice;
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
				manager->disconnectNotification(); // 1=non control eps
				break;

			case CONTROL_REQUEST_CONNECT:
				manager->connectNotification();
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
		// Zero Lenth Packets (ZLP) can be a problem.  If all ZLPs are
		// passed the performance will suffer and buffers may (will?)
		//  overflow.  ZLP canot be completly blocked as they are
		// required by in some cases by the usb protocols (i.e. used for
		// bulk transfers that are less than the requested size and a
		// multiple of the maximum packet size).  I have also found that
		// some drivers expect ZLP for no data and will have problems
		// if they just get NAKs (gadgetfs default way to present no
		// data).  To resolve this problem the code always sends the
		// first ZLP, then periodically sends them afterwards.
		//
		// Note: The setting of RelayReader::nice is heavily dependent
		//       upon the DeviceProxy::nice, so care must be taken to
		//       update RelayReader::nice whenever DeviceProxy::nice
		//       is changed.
		//
		// todo: review: this is only really needed  when reading from
		// the deviceProxy.  Should we change disable for hostProxy?
		else if (nullptr != buf) {
			if ((zlpCount % 4) == 0) {
				std::cout << "nullCount = " << zlpCount << std::endl;
				_sendQueue->enqueue(std::make_shared<Packet>(endpoint, buf, length));
				++zlpCount;
			} else {
				++zlpCount;
				free(buf);
			}

		} else {
			free(buf);
		}
	}
	fprintf(stderr,"Finished reader thread (%ld) for EP%02x.\n",gettid(),endpoint);
	_please_stop = false;
}
