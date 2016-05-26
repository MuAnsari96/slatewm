#ifndef SLATEWM_SLATE_H
#define SLATEWM_SLATE_H

#include <X11/Xlib.h>
#include <memory>

class Slate {
private:
    Display* display;
    Window root;
    Slate();

public:
    static std::unique_ptr<Slate> getInstance();
    ~Slate();
    void run();
};


#endif
