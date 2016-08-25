import zmq

def get_conn():
    '''
    Finds the context that connects to a running instance of slate, initing it if necessary
    :return: A context to the running instance of slate
    '''
    try:
        get_conn.towm
    except AttributeError as e:
        cxt = zmq.Context()
        get_conn.towm = cxt.socket(zmq.PUSH)
        get_conn.towm.connect("ipc:///tmp/slateclient")

    return get_conn.towm

class ToSlate:
    ''' An enum representing Slate Event '''
    KILL_FOCUSED_CLIENT = 0
    HIDE_FOCUSED_CLIENT = 1
    UNHIDE_FOCUSED_CLIENT = 2
    SWITCH_WORKSPACE = 3
    RESTYLE_ROOT = 4
    RESTYLE_CHILDREN = 5

class FromSlate:
    ''' An enum representing the client events '''
    DEFAULT = -1
    KEY_PRESS = 0
    KEY_RELEASE = 1
    GET_ROOT_WINDOW = 2
    GET_CHILD_WINDOWS = 3

def sendmsg(msg):
    '''
    Sends a message to slate
    :param msg: A string to be encoded as json, and sent to slate
    '''
    get_conn().send_json(msg)

