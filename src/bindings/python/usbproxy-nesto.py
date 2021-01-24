import usbproxy
from filters import log_filter

try:
    usbproxy.init()
    usbproxy.lib.set_config("DeviceProxy", "DeviceProxy_LibUSB")
    usbproxy.lib.set_config("productId", "07f8")
    usbproxy.lib.set_config("vendorId", "045e")
    #usbproxy.lib.enable_logging()
    usbproxy.lib.print_config()
    usbproxy.register_packet_filter(log_filter)
    usbproxy.lib.load_plugins()
    usbproxy.run()
except KeyboardInterrupt:
    pass