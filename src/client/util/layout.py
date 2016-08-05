class Window:
    def __init__(self, xcoords, ycoords, client = -1, style = None):
        self.xcoords = xcoords
        self.ycoords = ycoords
        self.client = client
        self.style = style


class WindowStyle:
    TILE = 0
    STACK = 1
    FLOAT = 2

    def __init__(self, id, behavior, primary_child = None, secondary_child = None):
        self.id = id

        self.behavior = behavior

        self.primary_child = primary_child
        self.secondary_child = secondary_child

    def __eq__(self, other):
        return self.id == other.id and \
            self.type == other.type and \
            self.primary_child == other.primary_child and \
            self.secondary_child == other.secondary_child

class Layout:
    def __init__(self, id, root_style):
        self.id = id
        self.root_style = root_style
        self.styles = {}
        self.add_style(root_style)

    def __getitem__(self, item):
        return self.styles[item]

    def __delitem__(self, key):
        del self.styles[key]

    def __contains__(self, item):
        if item is None:
            return True

        return item.id in self.styles

    def __iter__(self):
        return iter(self.styles.values())

    def add_style(self, style):
        self.styles[style.id] = style

    def validate(self):
        for style in self:
            if style.primary_child not in self:
                raise AssertionError("{} not a valid style in layout".format(style.primary_child))
            if style.secondary_child not in self:
                raise AssertionError("{} not a valid style in layout".format(style.secondary_child))

class LayoutManager:
    def __init__(self):
        self.current_layout = 0
        self.layouts = []

    def add_layout(self, layout):
        self.layouts.append(layout)

    def change_layout(self, count):
        self.current_layout = count % len(self.layouts)


def HorizontalTile(window):
    midpoint = (window.ycoords[0] + window.ycoords[1])//2
    return (Window(window.xcoords, (window.ycoords[0], midpoint)),
            Window(window.xcoords, (midpoint, window.ycoords[1])))

def VerticalTile(window):
    midpoint = (window.xcoords[0] + window.xcoords[1])//2
    return (Window((window.xcoords[0], midpoint), window.ycoords),
            Window((midpoint, window.xcoords[1]), window.ycoords))

def AutoTile(window):
    if window.ycoords[1] - window.ycoords[0] > window.xcoords[1] - window.xcoords[0]:
        return HorizontalTile(window)
    return VerticalTile(window)

TreeStyle = WindowStyle("tree", AutoTile, "tree", "tree")