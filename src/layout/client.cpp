#include <iostream>
#include "client.h"

Client::Client(int Xid) {
    this->Xid = Xid;
}

Window Client::clientID(Display *d, Window Xid) {
    Window root = XDefaultRootWindow(d);
    Window parent = Xid;
    Window *children;
    unsigned int numchildren;
    while (root != parent) {
        Xid = parent;
        XQueryTree(d, Xid, &root, &parent, &children, &numchildren);
        if (numchildren != 0) {
            XFree(children);
        }
    }
    return Xid;
}