#include <unistd.h>
#include "greengrasssdk.h"

#include "Manager.h"
#include "ConfigParser.h"
#include <signal.h>
#include <zmq.hpp>

Manager* manager;
zmq::context_t *ctx = new zmq::context_t();
zmq::socket_t *frontend = new zmq::socket_t(*ctx, zmq::socket_type::xsub);
zmq::socket_t *backend = new zmq::socket_t(*ctx, zmq::socket_type::xpub);
static int done;

void run_proxy(zmq::socket_t *frontend, zmq::socket_t *backend) {

	// proxy until context is destroyed, see: http://api.zeromq.org/3-2:zmq-proxy
	zmq::proxy(*frontend, *backend);
}

void handle_signal(int signum)
{
	(void)signum;
	done = 1;
}

void handler(const gg_lambda_context *cxt) {
    (void)cxt;
    return;
}

int main() {
    gg_error err = GGE_SUCCESS;

    err = gg_global_init(0);
    if(err) {
        gg_log(GG_LOG_ERROR, "gg_global_init failed %d", err);
        return -1;
    }

    gg_runtime_start(handler, GG_RT_OPT_ASYNC);

    struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = handle_signal;
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGINT, &action, NULL);

	const char* vendorId = std::getenv("VENDOR_ID");
	const char* productId = std::getenv("PRODUCT_ID");

    ConfigParser *cfg = new ConfigParser();

	if (vendorId != NULL) {
		const std::string vendorIdStr = vendorId;
    	cfg->set("vendorId", vendorIdStr);
	}

	if (productId != NULL) {
		const std::string productIdStr = productId;
    	cfg->set("productId", productIdStr);
	}

    cfg->set("DeviceProxy", "DeviceProxy_LibUSB");
    cfg->set("HostProxy", "HostProxy_GadgetFS");
	cfg->set("DeviceProxy::nice", "50");
    cfg->add_to_vector("Plugins", "PacketFilter_ZeroMQ");

    (*frontend).bind("tcp://127.0.0.1:9999");
	(*backend).bind("tcp://127.0.0.1:5678");
	std::thread zmq_proxy = std::thread(run_proxy, std::ref(frontend), std::ref(backend));

    int status;
	do {
		manager=new Manager(cfg);
		manager->load_plugins(cfg);
		cfg->print_config();

		manager->start_control_relaying();
		while ( ( status = manager->get_status()) == USBM_RELAYING) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		manager->stop_relaying();
		manager->cleanup();
	} while (!done && status == USBM_RESET);

	frontend->close();
	backend->close();
	zmq_proxy.join();
	ctx->close();
	delete(frontend);
	delete(backend);
	delete(manager);
	delete(ctx);

    return -1;
}