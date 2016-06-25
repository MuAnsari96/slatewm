from events import *
from rc import client

import zmq

def main():
    cxt = zmq.Context()
    fromwm = cxt.socket(zmq.PULL)
    fromwm.connect("ipc:///tmp/slateevents")
    event_loop(fromwm)

def event_loop(fromwm):
    while True:
        msg = fromwm.recv_json();
        event = msg["Event"]

        if (event == "KeyPress"):
            keys = msg["Keys"]
            keymask = "".join(list(map(chr, keys["Keymask"])))
            press = KeyPress(keys["Meta"], keys["Alt"], keys["Ctl"],
                             keys["Shift"], keys["Space"], keys["Enter"], keymask)
            client.handle_keypress(press)

if __name__ == "__main__":
    main()
