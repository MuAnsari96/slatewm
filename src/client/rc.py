from handler import Handler
from events import *
import util

client = Handler()
layout = client.layout_manager

client.register_keypress(
    KeyPress(alt=True, shift=True, mask='c'),
    callback=util.KillFocusedClient)

client.register_keypress(
    KeyPress(alt=True, enter=True, mask=''),
    callback=lambda: util.SpawnProcess("xterm"))

client.register_keypress(
    KeyPress(alt=True, mask='n'),
    callback=util.HideFocusedClient)

client.register_keypress(
    KeyPress(alt=True, shift=True, mask='n'),
    callback=util.UnhideFocusedClient)

client.register_keypress(
    KeyPress(alt=True, mask='1'),
    callback=lambda: util.SwitchWorkspace('0'))

client.register_keypress(
    KeyPress(alt=True, mask='2'),
    callback=lambda: util.SwitchWorkspace('b'))

client.register_keypress(
    KeyPress(alt=True, mask='3'),
    callback=lambda: util.SwitchWorkspace('c'))

layout.add_layout(util.Layout("auto", util.Window(), util.TreeStyle))
