const zmq = require("zeromq");

const sock = new zmq.Subscriber;

(async function() {
    sock.connect("tcp://127.0.0.1:5678");
    sock.subscribe();
    console.log("Subscriber connected to port 5678");
    let buf = [];
    let matched = 0;
    let desired = [0x1b, 0x6d]; // ESC/P cut sequence

    for await (const [msg] of sock) {
       for (const c of msg) {
	        buf.push(c);
          console.log(c);
          const expected = desired[matched];
          if (expected == c) {
            matched++;
	        } else {
            matched = 0;
          }

          if (matched == desired.length) {
	          console.log("CUT found. Receipt:\n");
            console.log(buf.map(s => String.fromCharCode(s)).join(""));
            matched = 0;
            buf = [];
	        }
       }
    }
})();
