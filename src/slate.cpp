#include "slate.h"

#include <iostream>

#include "util/message.h"
#include "util/util.h"

std::shared_ptr<Slate> Slate::instance = std::shared_ptr<Slate>();

Slate::Slate() :
    ctx(1), toclient(ctx, ZMQ_PUSH), fromclient(ctx, ZMQ_PULL) {

    // Magic 1.25 till we get a DPI fix
    state.workspaces["0"] = new Workspace(unsigned(int(GetSystemMetrics(SM_CXSCREEN)*1.25)), unsigned(int(GetSystemMetrics(SM_CYSCREEN)*1.25)), "0");
    state.focused_client = root;

    toclient.bind("ipc:///tmp/slateevents");
    fromclient.bind("ipc:///tmp/slateclient");

    Message::InitClientSocket(&toclient);
}

Slate::~Slate() {
}

std::shared_ptr<Slate> Slate::getInstance() {
    if (!instance)
        instance = std::shared_ptr<Slate>(new Slate);
    return instance;
}

void Slate::EventLoop() {
    /* An event loop that handles all relevant XEvents and requests updates from the client, if necessary
     */
    XSelectInput(display, root, KeyReleaseMask | KeyPressMask | SubstructureNotifyMask);
    while (true) {
        XEvent e;
        Util::InterruptibleXNextEvent(display, &e);

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
                state.workspaces[state.workspaceID]->setFocusedClient(e.xkey.window);

                Message::PopulateMessage(&jmsg, state, e);
                jmsg["Delta"] = xkeysym;
                jmsg["Event"] = Message::ToClient::KEY_PRESS ;
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
                jmsg["Event"] = Message::ToClient::KEY_RELEASE ;
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
                    w->getRoot()->drawTile();
                }
                Message::PopulateMessage(&jmsg, state, e);
                break;
            }
            case DestroyNotify: {
                Workspace *w = state.workspaces[Workspace::clientLUT[e.xdestroywindow.window]];
                if (w) {
                    w->removeClient(e.xdestroywindow.window);
                    w->getRoot()->drawTile();
                }
                break;
            }
            default:
                break;
        }
        Message::SendToClient(&jmsg);
    }
}

void Slate::EventLoopWrapper() {
    getInstance()->EventLoop();
}

zmq::socket_t& Slate::getClientPipe() {
    return fromclient;
}

const slate_state_t& Slate::getState() const {
    return state;
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
    std::unordered_set<HWND> clients = curr->clients;
    for (Window client: clients) {
        ShowWindow(client, SW_HIDE);
    }
}

void Slate::showWorkspace(std::string targetName) {
    if (state.workspaces.count(targetName) == 0) {

        // Magic 1.25 till we get a DPI fix
        state.workspaces[targetName] = new Workspace(unsigned(int(GetSystemMetrics(SM_CXSCREEN)*1.25)), unsigned(int(GetSystemMetrics(SM_CYSCREEN)*1.25)), "0");
    }
    Workspace *target = state.workspaces[targetName];
    std::unordered_set<HWND> clients = target->clients;
    for (Window client: clients) {
        ShowWindow(client, SW_SHOW);
    }
    state.workspaceID = targetName;
}
