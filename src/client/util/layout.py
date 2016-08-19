class WindowStyle:
    TILE = 0
    STACK = 1
    FLOAT = 2

    def __init__(self, name, behavior_fn, primary_style, secondary_style):
        self.name = name
        self.behavior_fn = behavior_fn

        self.primary_style = primary_style
        self.secondary_style = secondary_style

    def __eq__(self, other):
        return self.name == other.name

    def behavior(self, window):
        windows = self.behavior_fn(window)
        ret = [None, None]
        try:
            ret[0] = StyledWindow(window=windows[0], style=self.primary_style)
            ret[1] = StyledWindow(window=windows[1], style=self.secondary_style)
        except IndexError:
            pass
        return ret


class Window:
    def __init__(self, xcoords=[0,0], ycoords=[0,0], style_type=WindowStyle.TILE):
        self.xcoords = xcoords
        self.ycoords = ycoords
        self.style_type = style_type

    def flatten(self, id=0):
        return {"styleType": self.style_type,
                "xmin": self.xcoords[0],
                "xmax": self.xcoords[1],
                "ymin": self.ycoords[0],
                "ymax": self.ycoords[1],
                "id": id}


class StyledWindow(Window):
    def __init__(self, xcoords=[0,0], ycoords=[0,0], style_type=WindowStyle.TILE, style=None, window=None):

        if window is None:
            super(StyledWindow, self).__init__(xcoords, ycoords, style_type)
        else:
            self.xcoords = window.xcoords
            self.ycoords = window.ycoords
            self.style_type = window.style_type

        self.style = style

    def flatten(self, id=0):
        ret = super(StyledWindow, self).flatten(id)
        ret["style"] = self.style
        return ret


class Layout:
    def __init__(self, name, root_window, root_style):
        self.name = name
        self.root_window = StyledWindow(window=root_window, style=root_style.name)
        self.root_type = root_window.style_type

        self.root_style = root_style
        if (self.root_style is None):
            raise AssertionError("Root Window for layout must be defined with a style")

        self.styles = {}
        self.add_style(self.root_style)

    def __getitem__(self, item):
        return self.styles[item]

    def __delitem__(self, key):
        del self.styles[key]

    def __contains__(self, item):
        if item is None:
            return True

        return item.name in self.styles

    def __iter__(self):
        return iter(self.styles.values())

    def add_style(self, style):
        self.styles[style.name] = style


class LayoutManager:
    def __init__(self):
        self.current_layout = 0
        self.layouts = []

    def add_layout(self, layout):
        self.layouts.append(layout)

    def change_layout(self, count):
        self.current_layout = count % len(self.layouts)

    def get_current_layout(self):
        return self.layouts[self.current_layout]

    def split(self, window):
        return self.layouts[self.current_layout][window.style].behavior(window)


def HorizontalTile(window):
    midpoint = (window.ycoords[0] + window.ycoords[1])//2
    return (Window(window.xcoords, (window.ycoords[0], midpoint), WindowStyle.TILE),
            Window(window.xcoords, (midpoint, window.ycoords[1]), WindowStyle.TILE))

def VerticalTile(window):
    midpoint = (window.xcoords[0] + window.xcoords[1])//2
    return (Window((window.xcoords[0], midpoint), window.ycoords, WindowStyle.TILE),
            Window((midpoint, window.xcoords[1]), window.ycoords, WindowStyle.TILE))

def AutoTile(window):
    if window.ycoords[1] - window.ycoords[0] > window.xcoords[1] - window.xcoords[0]:
        return HorizontalTile(window)
    return VerticalTile(window)

TreeStyle = WindowStyle("TreeStyle", AutoTile, "TreeStyle", "TreeStyle")