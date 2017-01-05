#include "client.h"

Client::Client(Window Wid) {
    this->Wid = Wid;
}

Window Client::clientID(Window Wid) {
    Window temp = GetParent(Wid);
    while (temp != NULL) {
        Wid = temp;
        temp = GetParent(Wid);
    }
    return Wid;
}