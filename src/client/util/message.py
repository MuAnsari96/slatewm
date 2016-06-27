import zmq

def get_conn():
    try:
        get_conn.towm
    except AttributeError as e:
        cxt = zmq.Context()
        get_conn.towm = cxt.socket(zmq.PUSH)
        get_conn.towm.connect("ipc:///tmp/slateclient")

    return get_conn.towm

class Events:
    KILL_FOCUSED_CLIENT = 0
    HIDE_FOCUSED_CLIENT = 1
    UNHIDE_FOCUSED_CLIENT = 2
    SWITCH_WORKSPACE = 3


def sendmsg(msg):
    get_conn().send_json(msg)

