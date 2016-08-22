#ifndef SLATEWM_MESSAGE_H
#define SLATEWM_MESSAGE_H

#include <X11/Xlib.h>

#include "json.h"
#include "zmq.h"

#include "../slate.h"

using json = nlohmann::json;

namespace Message {

    enum ToClient {
        DEFAULT = -1,
        KEY_PRESS = 0,
        KEY_RELEASE = 1,
        GET_ROOT_WINDOW = 2,
        GET_CHILD_WINDOWS = 3
    };

    enum FromClient {
        Kill_FOCUSED_CLIENT = 0,
        HIDE_FOCUSED_CLIENT = 1,
        UNHIDE_FOCUSED_CLIENT = 2,
        SWITCH_WORKSPACE = 3,
        RESTYLE_ROOT = 4,
        RESTYLE_CHILDREN = 5
    };

    void PopulateMessage(json *j, const slate_state_t &state, const XEvent &e);
    void AppendToMessage(json *j, const Tile& tile);
    void AppendToMessage(json *j, const Tile& toSplit, const Tile& primary, const Tile& secondary);
    void InitClientSocket(zmq::socket_t* toClient);

    void SendToClient(json *j);
}

#endif //SLATEWM_MESSAGE_H
