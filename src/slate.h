#ifndef SLATEWM_SLATE_H
#define SLATEWM_SLATE_H

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <memory>
#include <set>
#include "zmq.hpp"

struct slate_state_t{
    bool meta = false;
    bool shift = false;
    bool ctl = false;
    bool alt = false;
    std::set<unsigned long> keymask;
    int workspace = 0;
};

class Slate {
private:
    Display* display;
    Window root;
    zmq::context_t ctx;
    zmq::socket_t toclient;
    slate_state_t state;
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
