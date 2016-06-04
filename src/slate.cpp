#include <iostream>
#include "slate.h"

#define ALT 64
#define CTL 37
#define MOD 133

Slate::Slate() :
    ctx(1), toclient(ctx, ZMQ_PUSH), fromclient(ctx, ZMQ_PULL) {
    Display* display_ = XOpenDisplay(nullptr);

    if (!display_) {
        std::cout << "Unable to open X display" << std::endl;
        return;
    }
    display = display_;
    root = DefaultRootWindow(display_);

    //toclient.bind("ipc:///tmp/feeds/0");
    //fromclient.bind("ipc:///tmp/feeds/1");
}

Slate::~Slate() {
    XCloseDisplay(display);
}

std::unique_ptr<Slate> Slate::getInstance() {
    return std::unique_ptr<Slate> (new Slate);
}

void Slate::run() {
    XSelectInput(display, root, KeyReleaseMask | KeyPressMask | SubstructureNotifyMask);
    // ALT, CTL, MOD
    bool mods[3] = {false, false, false};
    while (true) {
        XEvent e;
        XNextEvent(display, &e);

        switch (e.type) {
            case KeyPress:
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
