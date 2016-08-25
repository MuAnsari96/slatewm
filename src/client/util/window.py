''' These functions all emit messages to slate to handle the general management of
    windows contained in the wm. '''
from util.message import get_conn
from util.message import ToSlate

towm = get_conn()

def KillFocusedClient():
    msg = {"Event": ToSlate.KILL_FOCUSED_CLIENT}
    towm.send_json(msg)

def HideFocusedClient():
    msg = {"Event": ToSlate.HIDE_FOCUSED_CLIENT}
    towm.send_json(msg)

def UnhideFocusedClient():
    msg = {"Event": ToSlate.UNHIDE_FOCUSED_CLIENT}
    towm.send_json(msg)

def SwitchWorkspace(workspaceid):
    msg = {"Event": ToSlate.SWITCH_WORKSPACE, "Workspace": workspaceid}
    towm.send_json(msg)