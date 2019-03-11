/*
 * This file is part of USBProxy.
 */

#include <poll.h>
#include <stdio.h>
#include <sched.h>
#include "get_tid.h"
#include "TRACE.h"

#include "Injector.h"

#define SLEEP_US 1000

Injector::Injector()
	: outPollIndex{},
	  inPollIndex{},
	  _please_stop(false),
	  outQueues{},
	  inQueues{}
{}

void Injector::set_queue(__u8 epAddress, PacketQueue& queue) {
	if (epAddress&0x80) {
		inQueues[epAddress&0x0f] = &queue;
	} else {
		outQueues[epAddress&0x0f] = &queue;
	}
}

void Injector::listen() {
	bool idle;
	fprintf(stderr,"Starting injector thread (%ld) for [%s].\n",gettid(),this->toString());
	start_injector();
	Packet* packet;
	SetupPacket* setup;
	int* fdlist=get_pollable_fds();
	int pollreadcount=0;
	int i=0;
	while(fdlist[i++]) {pollreadcount++;}
	struct pollfd* poll_list=(struct pollfd*)calloc(pollreadcount,sizeof(pollfd));
	for (i=0;i<pollreadcount;i++) {
		fprintf(stderr,"Injector In FD[%d/%d]: %d\n",i+1,pollreadcount,fdlist[i]);
		poll_list[i].fd=fdlist[i];
		poll_list[i].events=POLLIN;
	}
	free(fdlist);


	while (!_please_stop) {
		idle=true;
		if (poll(poll_list,pollreadcount,500)) {
			for(i=0;i<pollreadcount;i++) {
				// note the following is a bit strange to handle
				// oddities with keeping legacy injectors
				// compatible with the shared pointers
				if (poll_list[i].revents & POLLIN) {
					get_packets(&packet,&setup,500);
					if (setup) {
						PacketQueue* queue=outQueues[0];
						if (queue) {
							fprintf(stderr,"Injector send setup on %d\n",i);
							//queue->enqueue(setup);
							queue->enqueuePriority(std::make_shared<SetupPacket>(*setup));
							setup->data = nullptr;
							delete setup;
							idle=false;
						} else {
							full_pipe(setup);
							idle=false;
						}
						setup=nullptr;
					} else if (packet) {
						__u8 epAddress=packet->bEndpoint;
						PacketQueue* queue =(epAddress&0x80)?inQueues[epAddress&0x0f]:outQueues[epAddress&0x0f];
						if (queue) { //if queue defined for this EP, attempt to send
							//queue->enqueue(packet);
							queue->enqueuePriority(std::make_shared<Packet>(*packet));

							packet->data = nullptr;
							delete packet;
						} else { //kick packet back to injector
							full_pipe(packet);
						}
						packet = nullptr;
						idle=false;
					}
				}
			}
		}
		if (idle) sched_yield();
	}
	free(poll_list);
	stop_injector();
	fprintf(stderr,"Finished injector thread (%ld) for [%s].\n",gettid(),this->toString());
	_please_stop = false;
}
