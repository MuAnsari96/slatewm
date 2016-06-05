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
    static std::shared_ptr<Slate> instance;

    Slate();
    void XEventLoop();
    void ClientLoop();

public:
    zmq::socket_t fromclient;

    ~Slate();
    static std::shared_ptr<Slate> getInstance();
    static void XEventLoopWrapper();
    static void ClientLoopWrapper();

};


#endif
