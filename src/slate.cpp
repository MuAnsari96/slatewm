#include <iostream>
#include <X11/Xlib.h>
#include "slate.h"
#include "client_handler.h"
#include "message.h"

#define ALT 64
#define CTL 37
#define META 133
#define SHIFT 50

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

    toclient.bind("ipc:///tmp/slateevents");
    fromclient.bind("ipc:///tmp/slateclient");
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
        XNextEvent(display, &e);

        json jmsg;
        unsigned long xkeysym;
        switch (e.type) {
            case KeyPress:
                state.alt = e.xkey.keycode == ALT ? true: state.alt;
                state.ctl= e.xkey.keycode == CTL ? true: state.ctl;
                state.meta = e.xkey.keycode == META ? true: state.meta;
                state.shift = e.xkey.keycode == SHIFT ? true: state.shift;

                xkeysym = XkbKeycodeToKeysym(display, e.xkey.keycode, 0, 0);
                if (xkeysym < 128)
                    state.keymask.insert(xkeysym);

                Message::PopulateMessage(&jmsg, state);
                jmsg["Delta"] = xkeysym;
                jmsg["Event"] = "KeyPress";
                break;
            case KeyRelease:
                state.alt = e.xkey.keycode == ALT ? false: state.alt;
                state.ctl= e.xkey.keycode == CTL ? false: state.ctl;
                state.meta = e.xkey.keycode == META ? false: state.meta;
                state.shift = e.xkey.keycode == SHIFT ? false: state.shift;

                xkeysym = XkbKeycodeToKeysym(display, e.xkey.keycode, 0, 0);
                state.keymask.erase(xkeysym);

                Message::PopulateMessage(&jmsg, state);
                jmsg["Event"] = "KeyRelease";
                break;
            case CreateNotify:
                XSetWindowAttributes attr;
                attr.do_not_propagate_mask = 0;
                attr.event_mask = KeyReleaseMask | KeyPressMask | SubstructureNotifyMask;
                XChangeWindowAttributes(display, e.xcreatewindow.window, CWDontPropagate | CWEventMask, &attr);
                Message::PopulateMessage(&jmsg, state);
                break;
            default:
                break;
        }
        std::string jmsg_str = jmsg.dump();
        zmq::message_t msg(jmsg_str.size());
        memcpy(msg.data(), jmsg_str.c_str(), jmsg_str.size());
        toclient.send(msg, ZMQ_NOBLOCK);
    }
}

void Slate::ClientLoop() {
    ClientHandler::Run(this);
}

void Slate::XEventLoopWrapper() {
    getInstance()->XEventLoop();
}

void Slate::ClientLoopWrapper() {
    getInstance()->ClientLoop();
}
