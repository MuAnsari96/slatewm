import zmq

def get_conn():
    try:
        get_conn.towm
    except AttributeError as e:
        cxt = zmq.Context()
        get_conn.towm = cxt.socket(zmq.PUSH)
        get_conn.towm.connect("ipc:///tmp/slateclient")

    return get_conn.towm

class ToSlate:
    KILL_FOCUSED_CLIENT = 0
    HIDE_FOCUSED_CLIENT = 1
    UNHIDE_FOCUSED_CLIENT = 2
    SWITCH_WORKSPACE = 3

class FromSlate:
    DEFAULT = -1
    KEY_PRESS = 0
    KEY_RELEASE = 1
    FILL_ROOT_WINDOW = 2
    SPLIT_WINDOW = 3
    RECALCULATE_ROOT_BOUNDARY = 4
    RECALCULATE_BOUNDARIES = 5

def sendmsg(msg):
    get_conn().send_json(msg)

