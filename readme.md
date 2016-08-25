# Slate(wm)
A simple window management framework for X

## Overview
Serving as an abstraction layer resting on top of the X server, Slate is essentially a framework for X 
window managers. Users can define various clients that connect to a Slate instance in order to define the 
behavior of windows on their screen. 

Using a bundled library, a single python configuration file can be used to determine the behavior 
the Slate stack, by defining window layouts, workspaces, keybindings, and more. Thus, when paired with a
configuration file, Slate serves as a fully configurable window manager. Since the built-in framework
allows for configuration files to be written in python, this means that the entire window manager can be
dynamically scripted, and the window manager can leverage the power of the entire python programming language.
This also means that users can dynamically open a connection to Slate through a REPL, or even programmatically 
interact with and manage X clients.

## Current Status
At this time, slate is able to register keybindings to open and close windows, manage workspaces, 
and allows for user-defined tiled window management. In the future, I plan to implement:
* "Drawable" Objects, like menus and taskbars
* Support for stacking and floating windows

Since it is right now in very active development, there are no install rules for the project. Instead, the way
to run the slate stack is to build and run slate, and then running the client, contained in`src/client/slateclient.py`.
Since slate is at the point where it could reasonably used as a minimalistic window manager, installation support is 
planned to be completed soon!

## More Information on Slate
Currently, Slate is composed of two parts: slate and slateclient. 

As mentioned above, slate serves as an 
abstraction layer on top of the X server. Traditionally, the X window system is concerned with a very low
level interpretation of input and output; X keeps track of raw input events, and concerns itself with drawing 
rectangles to the screen. Slate provides a series of abstractions to keep track of high level inputs and outputs
(eg, chords, window positions, layouts, workspaces), and communicates these abstractions to and from X.

Slateclient is what maps high level inputs to high level outputs. This is what most people come to think of 
when discussing window managers. The "main" slateclient, which runs alongside the slate instance, is defined in
`src/slateclient/rc.py`. This file essentially defines the complete behavior of the slate window management system,
and so together, slate and the client defined in rc.py are collectively referred to as slatewm.

Thus, the slate stack works in concert with the X system in order to distill the entire behavior of a window manager 
into a single, easily configurable file that can be dynamically loaded and reloaded. The X server relays raw input to 
slate, which translates the input to reasonable user behavior. The client determines a reasonable response to the 
behavior, and relays it to slate, which in turn translates the output into events understood by the X server. Using
slate, then, writing a window manager is as simple as writing a single python file.