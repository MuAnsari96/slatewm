#include "client.h"

Client::Client(int Xid) {
    this->Xid = Xid;
}

Window Client::clientID(Display *d, Window Xid) {
    /* Queries the XTree in order to determine the top-most client id of a given client; this is necessary, as all
     * rectangles contained in a displayed client are considered clients in their own right by X, and therefore get
     * an ID. The window manager, however, need only concern itself the topmost client enclosing.
     */
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