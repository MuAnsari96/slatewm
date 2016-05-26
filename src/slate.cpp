#include <iostream>
#include "slate.h"
#include "event_handler.h"

Slate::Slate() {
    Display* display_ = XOpenDisplay(nullptr);

    if (!display_) {
        std::cout << "Unable to open X display" << std::endl;
        return;
    }
    display = display_;
    root = DefaultRootWindow(display_);
}

Slate::~Slate() {
    XCloseDisplay(display);
    delete display;
}

static std::unique_ptr<Slate> Slate::getInstance() {
    return std::unique_ptr<Slate> (new Slate);
}

void Slate::run() {
    using namespace XEventHandler;
    while (true) {
        XEvent e;
        XNextEvent(display, &e);

        switch (e.type) {
            case CreateNotify:
                // call stuff from XEventHandler, for example
                break;
            default:
                return;
        }
    }
}
