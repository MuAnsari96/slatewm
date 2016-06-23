import zmq

def get_conn():
    try:
        get_conn.towm
    except AttributeError as e:
        cxt = zmq.Context()
        get_conn.towm = cxt.socket(zmq.PUSH)
        get_conn.towm.connect("ipc:///tmp/slateclient")

    return get_conn.towm

towm = get_conn()

def sendmsg(msg):
    towm.send_json(msg)