from util.message import get_conn

towm = get_conn()

def KillFocusedClient():
    d = {"Event": "CloseFocusedClient"}
    towm.send_json(d)
