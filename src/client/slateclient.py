import zmq
import message
import rc as userconf

def main():
    cxt = zmq.Context()
    fromwm = cxt.socket(zmq.PULL)
    fromwm.connect("ipc:///tmp/slateevents")
    event_loop(fromwm)

def event_loop(fromwm):
    while True:
        msg = fromwm.recv_json();
        print(msg)
        message.talk()


if __name__ == "__main__":
    main()
