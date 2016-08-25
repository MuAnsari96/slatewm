class WindowStyle:
    '''
    Describes the behavior of a "window" in slate; for the purposes of the wm, a window is the displayed container
    of any running client. A complete style represents a name, behavior (for when "Window A" is spawned while the focus
    is in a window with a given style), and the names of the styles of the children of this style.
    '''
    TILE = 0
    STACK = 1
    FLOAT = 2

    def __init__(self, name, behavior_fn, primary_style, secondary_style):
        '''
        Defines a window style
        :param name: Used to refer to a given style
        :param behavior_fn: Defines how windows behave when a new window is spawned within a styled window. Should
                            be a callable that returns a list of 2 windows representing (in order) the old client, and
                            the new one.
        :param primary_style: The name of the "primary" child's style-- in most cases, this is the previously styled
                              window.
        :param secondary_style: The name of the "secondary" child's style-- in most cases, the newly spawned window.
        '''
        self.name = name
        self.behavior_fn = behavior_fn

        self.primary_style = primary_style
        self.secondary_style = secondary_style

    def __eq__(self, other):
        return self.name == other.name

    def behavior(self, window):
        '''
        Wraps the behavior_fn callable.
        '''
        windows = self.behavior_fn(window)
        ret = [None, None]
        try:
            ret[0] = StyledWindow(window=windows[0], style=self.primary_style)
            ret[1] = StyledWindow(window=windows[1], style=self.secondary_style)
        except IndexError:
            pass
        return ret


class Window:
    '''
    Represents a container for clients displayed by the wm; this is essentially a bounding box, as well
    as a style type.
    '''
    def __init__(self, xcoords=[0,0], ycoords=[0,0], style_type=WindowStyle.TILE):
        '''
        Instantiates a new window
        :param xcoords: a tuple of the minimum and maximum x values of the window.
        :param ycoords: a tuple of the minimum and maximum y values of the window.
        :param style_type: describes whether the window is tiled, floating or stacking.
        '''
        self.xcoords = xcoords
        self.ycoords = ycoords
        self.style_type = style_type

    def flatten(self, id=0):
        '''
        Flattens a window to a JSON string.
        :param id: The id of the client contained by the window
        :return: A JSON string representation of the window
        '''
        return {"styleType": self.style_type,
                "xmin": self.xcoords[0],
                "xmax": self.xcoords[1],
                "ymin": self.ycoords[0],
                "ymax": self.ycoords[1],
                "id": id}


class StyledWindow(Window):
    '''
    A simple class that bundles a WindowStyle into the Window.
    '''
    def __init__(self, xcoords=[0,0], ycoords=[0,0], style_type=WindowStyle.TILE, style=None, window=None):
        '''
        Instantiates a new styled window
        :param xcoords: a tuple of the minimum and maximum x values of the window.
        :param ycoords: a tuple of the minimum and maximum y values of the window.
        :param style_type: describes whether the window is tiled, floating or stacking.
        :param style: the window style of the window
        :pawam window: optional argument that constructs other arguments from a window object.
        '''

        if window is None:
            super(StyledWindow, self).__init__(xcoords, ycoords, style_type)
        else:
            self.xcoords = window.xcoords
            self.ycoords = window.ycoords
            self.style_type = window.style_type

        self.style = style

    def flatten(self, id=0):
        '''
        Flattens a window to a JSON string.
        :param id: The id of the client contained by the window
        :return: A JSON string representation of the window
        '''
        ret = super(StyledWindow, self).flatten(id)
        ret["style"] = self.style
        return ret


class Layout:
    '''
    A hierarchy of inter-referential WindowStyles that define how to arbitrarily create windows.
    '''
    def __init__(self, name, root_window, root_style):
        '''
        Instantiates a Layout
        :param name: used to refer to a layout
        :param root_window: A Window that represents the starting size of a window in this layout.
        :param root_style: The style of the root window
        '''
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
        '''
        Adds a style to the layout.
        :param style: The style to be added
        '''
        self.styles[style.name] = style


class LayoutManager:
    '''
    The system that manages all the layouts exposed to the wm, and handles their behavior.
    '''
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


# Here are a few pre-written layout helpers that should further elucidate how all of the above
# classes come together in order to create a scriptable layout system. They can also be used to
# construct most simple tree-based tiled window management layouts.
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