#include "slate.h"

#include <iostream>

#include "util/message.h"
#include "util/util.h"

#define ALT 64
#define CTL 37
#define META 133
#define SHIFT 50
#define SPACE 65
#define ENTER 36

std::shared_ptr<Slate> Slate::instance = std::shared_ptr<Slate>();

Slate::Slate() :
    ctx(1), toclient(ctx, ZMQ_PUSH), fromclient(ctx, ZMQ_PULL) {
    Display* display_ = XOpenDisplay(nullptr);

    if (!display_) {
        std::cout << "Unable to open X display" << std::endl;
        return;
    }
    display = display_;
    root = DefaultRootWindow(display_);

    XWindowAttributes attr;
    XGetWindowAttributes(display, root, &attr);
    state.workspaces["0"] = new Workspace(attr.width, attr.height, "0");
    state.focused_client = root;

    toclient.bind("ipc:///tmp/slateevents");
    fromclient.bind("ipc:///tmp/slateclient");

    Message::InitClientSocket(toclient);
}

Slate::~Slate() {
    XCloseDisplay(display);
}

std::shared_ptr<Slate> Slate::getInstance() {
    if (!instance)
        instance = std::shared_ptr<Slate>(new Slate);
    return instance;
}

void Slate::XEventLoop() {
    XSelectInput(display, root, KeyReleaseMask | KeyPressMask | SubstructureNotifyMask);
    while (true) {
        XEvent e;
        InterruptibleXNextEvent(display, &e);

        json jmsg;
        Message::PopulateMessage(&jmsg, state, e);
        unsigned long xkeysym;
        switch (e.type) {
            case KeyPress: {
                state.alt = e.xkey.keycode == ALT ? true : state.alt;
                state.ctl = e.xkey.keycode == CTL ? true : state.ctl;
                state.meta = e.xkey.keycode == META ? true : state.meta;
                state.shift = e.xkey.keycode == SHIFT ? true : state.shift;
                state.space = e.xkey.keycode == SPACE ? true : state.space;
                state.enter = e.xkey.keycode == ENTER ? true : state.enter;

                xkeysym = XkbKeycodeToKeysym(display, e.xkey.keycode, 0, 0);
                if (xkeysym < 128)
                    state.keymask.insert(xkeysym);
                state.focused_client = e.xkey.window;
                state.workspaces[state.workspaceID]->focused_client = e.xkey.window;

                Message::PopulateMessage(&jmsg, state, e);
                jmsg["Delta"] = xkeysym;
                jmsg["Event"] = "KeyPress";
                break;
            }
            case KeyRelease: {
                state.alt = e.xkey.keycode == ALT ? false : state.alt;
                state.ctl = e.xkey.keycode == CTL ? false : state.ctl;
                state.meta = e.xkey.keycode == META ? false : state.meta;
                state.shift = e.xkey.keycode == SHIFT ? false : state.shift;
                state.space = e.xkey.keycode == SPACE ? false : state.space;
                state.enter = e.xkey.keycode == ENTER ? false : state.enter;

                xkeysym = XkbKeycodeToKeysym(display, e.xkey.keycode, 0, 0);
                state.keymask.erase(xkeysym);

                Message::PopulateMessage(&jmsg, state, e);
                jmsg["Event"] = "KeyRelease";
                break;
            }
            case CreateNotify: {
                XSetWindowAttributes attr;
                attr.do_not_propagate_mask = 0;
                attr.event_mask = KeyReleaseMask | KeyPressMask | SubstructureNotifyMask;
                XChangeWindowAttributes(display, e.xcreatewindow.window, CWDontPropagate | CWEventMask, &attr);
                Workspace* w = state.workspaces[state.workspaceID];
                if (w) {
                    w->addClient(display, e.xcreatewindow.window);
                    w->root->drawTile(display);
                }
                Message::PopulateMessage(&jmsg, state, e);
                break;
            }
            case DestroyNotify: {
                Workspace *w = state.workspaces[Workspace::clientLUT[e.xdestroywindow.window]];
                if (w) {
                    w->removeClient(e.xdestroywindow.window);
                    w->root->drawTile(display);
                }
                break;
            }
            default:
                break;
        }
        Message::SendToClient(&jmsg);
    }
}

void Slate::XEventLoopWrapper() {
    getInstance()->XEventLoop();
}

void Slate::switchToWorkspace(std::string targetName) {
    if (state.workspaceID == targetName) {
        return;
    }
    hideWorkspace();
    showWorkspace(targetName);
}

void Slate::hideWorkspace() {
    Workspace *curr = state.workspaces[state.workspaceID];
    std::unordered_set<unsigned int> clients = curr->clients;
    for (Window client: clients) {
        XUnmapWindow(display, client);
    }
}

void Slate::showWorkspace(std::string targetName) {
    if (state.workspaces.count(targetName) == 0) {
        XWindowAttributes attr;
        XGetWindowAttributes(display, root, &attr);
        state.workspaces[targetName] = new Workspace(attr.width, attr.height, targetName);
    }
    Workspace *target = state.workspaces[targetName];
    std::unordered_set<unsigned int> clients = target->clients;
    for (Window client: clients) {
        XMapWindow(display, client);
    }
    state.workspaceID = targetName;
}
