#ifndef SLATEWM_CLIENT_H
#define SLATEWM_CLIENT_H
#include <X11/Xlib.h>

class Client {
public:
    int Xid;

    Client(int Xid);
    static Window clientID(Display *d, Window Xid);
};


#endif //SLATEWM_CLIENT_H
