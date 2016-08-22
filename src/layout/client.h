#ifndef SLATEWM_CLIENT_H
#define SLATEWM_CLIENT_H

#include <X11/Xlib.h>

class Client {
public:
    Client(int Xid);

    int Xid;

    static Window clientID(Display *d, Window Xid);
};

#endif //SLATEWM_CLIENT_H
