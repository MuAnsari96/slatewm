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
            print(window)
            [primary, secondary] = \
                client.layout_manager.split(
                    util.StyledWindow(xcoords=(window["xmin"], window["xmax"]),
                                      ycoords=(window["ymin"], window["ymax"]),
                                      style_type=window["styleType"],
                                      style=window["style"]))
            msg = dict()
            msg["Event"] = util.ToSlate.RESTYLE_CHILDREN
            if primary is not None:
                msg["Primary"] = primary.flatten(window["primaryID"])
            if secondary is not None:
                msg["Secondary"] = secondary.flatten(window["secondaryID"])
            util.sendmsg(msg)

        elif event == util.FromSlate.DEFAULT:
            pass

if __name__ == "__main__":
    main()
