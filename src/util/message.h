#ifndef SLATEWM_MESSAGE_H
#define SLATEWM_MESSAGE_H

#include <X11/Xlib.h>

#include "json.h"
#include "zmq.h"


#include "../slate.h"

using json = nlohmann::json;

namespace Message {
    void PopulateMessage(json *j, const slate_state_t &state, const XEvent &e);
    void AppendToMessage(json *j, const Tile& tile);
    void AppendToMessage(json *j, const Tile& toSplit, const Tile& primary, const Tile& secondary);
    void InitClientSocket(const zmq::socket_t& toClient);

    void SendToClient(json *j);

    zmq::socket_t* client = nullptr;
}
#endif //SLATEWM_MESSAGE_H
