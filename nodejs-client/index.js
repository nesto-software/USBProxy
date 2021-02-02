const zmq = require("zeromq");
const msgpack = require('msgpack5')();

const sock = new zmq.Subscriber;

(async function() {
    sock.connect("tcp://127.0.0.1:5678");
    sock.subscribe();
    console.log("Subscriber connected to port 5678");

    for await (const [msg] of sock) {
        const msg_decoded = msgpack.decode(msg);
        console.log(msg_decoded.d.toString());
    }
})();