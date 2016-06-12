class KeyPress:
    def __init__(self, mod=False,
                 alt=False,
                 ctl=False,
                 shift=False,
                 mask=""):
        self.mods = [mod, alt, ctl, shift]
        self.mask = list(filter(str.isalnum, set(sorted(mask.lower()))))

    def __repr__(self):
        rep = "Mod + "*self.mods[0] + "Alt + "*self.mods[1] + \
               "Ctl + "*self.mods[2] + "Shift + "*self.mods[3] + \
               " + ".join(self.mask)
        if rep[-1] == ' ':
            return rep[:-2]
        return rep

    def __str__(self):
        return self.__repr__();

    def __eq__(self, other):
        return self.tuple_rep() == other.tuple_rep()

    def __hash__(self):
        return hash(self.tuple_rep())

    def tuple_rep(self):
        return tuple(self.mods) + tuple(self.mask)
