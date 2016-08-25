from util.layout import LayoutManager

class Handler:
    '''
    Represents an object that maps user events to wm actions
    '''

    def __init__(self):
        self.keybindings = {}
        self.layout_manager = LayoutManager()

    def handle_keypress(self, press):
        if (press in self.keybindings):
            self.keybindings[press]()

    def register_keypress(self, press, callback):
        if (callable(callback)):
            self.keybindings[press] = callback
        else:
            print("Tried to register noncallable callback")

