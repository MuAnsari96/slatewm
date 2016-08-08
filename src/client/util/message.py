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
    RESTYLE_ROOT = 4
    RESTYLE_CHILDREN = 5

class FromSlate:
    DEFAULT = -1
    KEY_PRESS = 0
    KEY_RELEASE = 1
    GET_ROOT_WINDOW = 2
    GET_CHILD_WINDOWS = 3

def sendmsg(msg):
    get_conn().send_json(msg)

