#ifndef SLATEWM_CLIENT_H
#define SLATEWM_CLIENT_H

#include <windows.h>

typedef HWND Window;

class Client {
public:
    Client(Window Wid);

    Window Wid;

    static Window clientID(Window Wid);
};

#endif //SLATEWM_CLIENT_H
