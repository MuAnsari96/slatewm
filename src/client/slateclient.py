from events import *
import util
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

        if event == util.FromSlate.KEY_PRESS:
            keys = msg["Keys"]
            keymask = "".join(list(map(chr, keys["Keymask"])))
            press = KeyPress(keys["Meta"], keys["Alt"], keys["Ctl"],
                             keys["Shift"], keys["Space"], keys["Enter"], keymask)
            client.handle_keypress(press)

        elif event == "SizeResize":
            window = msg["Window"]
            primary, secondary = \
                client.layout_manager.apply(
                    util.Window((window["xmin"], window["xmax"]),
                                (window["ymin"], window["ymax"]),
                                window["client"],
                                window["style"]))
            pass

if __name__ == "__main__":
    main()
