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

        elif event == util.FromSlate.KEY_RELEASE:
            pass

        elif event == util.FromSlate.GET_ROOT_WINDOW:
            window = msg["Window"]
            msg = dict()
            msg["Event"] = util.ToSlate.RESTYLE_ROOT
            msg["Root"] = client.layout_manager.get_current_layout().root_window.flatten(window["primaryID"])
            util.sendmsg(msg)

        elif event == util.FromSlate.GET_CHILD_WINDOWS:
            window = msg["Window"]
            primary, secondary = \
                client.layout_manager.apply(
                    util.Window((window["xmin"], window["xmax"]),
                                (window["ymin"], window["ymax"]),
                                window["style"],
                                window["styleType"]))
            msg = dict()
            msg["Event"] = util.ToSlate.RESTYLE_CHILDREN
            msg["Primary"] = primary.flatten(window["primaryID"])
            msg["Secondary"] = secondary.flatten(window["secondaryID"])
            util.sendmsg(msg)

        elif event == util.FromSlate.DEFAULT:
            pass

if __name__ == "__main__":
    main()
