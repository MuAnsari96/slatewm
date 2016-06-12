class Handler:

    def __init__(self):
        self.keybindings = {}

    def handle_keypress(self, press):
        if (press in self.keybindings):
            self.keybindings[press]()

    def register_keypress(self, press, callback):
        if (callable(callback)):
            self.keybindings[press] = callback
        else:
            print("Tried to register noncallable callback")
