#ifndef SLATEWM_SLATE_H
#define SLATEWM_SLATE_H

#include <X11/Xlib.h>
#include <memory>
#include "zmq.hpp"

class Slate {
private:
    Display* display;
    Window root;
    zmq::context_t ctx;
    zmq::socket_t toclient;
    zmq::socket_t fromclient;
    Slate();

public:
    static std::unique_ptr<Slate> getInstance();
    ~Slate();
    void run();
};


#endif
