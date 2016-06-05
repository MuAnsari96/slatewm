#include <iostream>
#include "slate.h"
#include "client_handler.h"

#define ALT 64
#define CTL 37
#define MOD 133

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
    // ALT, CTL, MOD
    bool mods[3] = {false, false, false};
    while (true) {
        XEvent e;
        XNextEvent(display, &e);

        zmq::message_t msg(5);
        memcpy(msg.data(), "PRESS!", 6);
        switch (e.type) {
            case KeyPress:
                toclient.send(msg, ZMQ_NOBLOCK);
                if (e.xkey.keycode == ALT) {
                    mods[0] = true;
                }
                if (e.xkey.keycode == CTL) {
                    mods[1] = true;
                }
                if (e.xkey.keycode == MOD) {
                    mods[2] = true;
                }
                break;
            case KeyRelease:
                if (e.xkey.keycode == ALT) {
                    mods[0] = true;
                }
                if (e.xkey.keycode == CTL) {
                    mods[1] = true;
                }
                if (e.xkey.keycode == MOD) {
                    mods[2] = true;
                }
                break;
            case CreateNotify:
                XSetWindowAttributes attr;
                attr.do_not_propagate_mask = 0;
                attr.event_mask = KeyReleaseMask | KeyPressMask | SubstructureNotifyMask;
                XChangeWindowAttributes(display, e.xcreatewindow.window, CWDontPropagate | CWEventMask, &attr);
                break;
            default:
                break;
        }
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
