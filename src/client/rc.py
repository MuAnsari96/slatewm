from handler import Handler
from events import *

client = Handler()

client.register_keypress(
    KeyPress(alt=True, shift=True, mask='j'),
    callback=lambda: print("hi"))
