const zmq = require("zeromq");
const msgpack = require('msgpack5')();

const sock = new zmq.Subscriber;

(async function() {
    sock.connect("tcp://127.0.0.1:5678");
    sock.subscribe();
    console.log("Subscriber connected to port 5678");
    let buf = [];
    let matched = 0;
    let desired = [0x1d, 0x56, 0x31];   // toshiba is set, epson would be: [0x1b, 0x6d]
    for await (const [msg] of sock) {
       for(const c of msg) {
          // console.log(c); // uncomment if you do not know the sequence, otherwise nothing is shown
	        buf.push(c);
          const expected = desired[matched];
          if(expected == c) {
            matched++;
	  } else {
            matched = 0;
          }

          if(matched == 2) {
	    console.log("CUT found. Receipt:\n");
	    console.log(buf.map(s => String.fromCharCode(s)).join(""));
	    matched = 0;
            buf = [];
	  }
	  //console.log(c.toString(16));
          //console.log(c.toString());
	  //if(buf.length === 100) {
	    //console.log(buf.map(s => s.toString(16)));
            //console.log(buf.map(s => String.fromCharCode(s)).join(""));
            //console.log(Buffer.from(buf));
	    //buf = [];
          //}
       }

       //console.log(buf.map(s => s.toString(16)));
       //console.log(buf.map(s => String.fromCharCode(s)).join(""));
       // const msg_decoded = msgpack.decode(msg);
       // console.log(msg_decoded.d);
    }
})();
