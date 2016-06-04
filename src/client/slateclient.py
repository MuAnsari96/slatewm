import zmq

def main():
    cxt = zmq.Context()
    fromwm = cxt.socket(zmq.PULL)
    fromwm.connect("ipc:///tmp/slateevents")
    event_loop(fromwm)

def event_loop(fromwm):
    while True:
        msg = fromwm.recv_string();
        print(msg)


if __name__ == "__main__":
    main()
