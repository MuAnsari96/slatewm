from util.message import get_conn
from util.message import Events

towm = get_conn()

def KillFocusedClient():
    msg = {"Event": Events.KILL_FOCUSED_CLIENT}
    towm.send_json(msg)

def HideFocusedClient():
    msg = {"Event": Events.HIDE_FOCUSED_CLIENT}
    towm.send_json(msg)

def UnhideFocusedClient():
    msg = {"Event": Events.UNHIDE_FOCUSED_CLIENT}
    towm.send_json(msg)
