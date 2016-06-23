from handler import Handler
from events import *
import util

client = Handler()

client.register_keypress(
    KeyPress(alt=True, shift=True, mask='c'),
    callback=util.KillFocusedClient)

client.register_keypress(
    KeyPress(alt=True, enter=True, mask=''),
    callback=lambda: util.SpawnProcess("/usr/bin/xterm"))
