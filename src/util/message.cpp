#include "message.h"

void Message::PopulateMessage(json *j, const slate_state_t &state, const XEvent &e) {
    (*j)["Event"] = "Default";
    (*j)["Client"] = state.focused_client;
    (*j)["Workspace"] = 0;
    (*j)["Screen"] = 0;
    (*j)["Keys"] = {
            {"Meta", state.meta},
            {"Shift", state.shift},
            {"Ctl", state.ctl},
            {"Alt", state.alt},
            {"Space", state.space},
            {"Enter", state.enter},
            {"Keymask", state.keymask},
            {"Delta", ""}
    };
}

void Message::AppendToMessage(json *j, const Tile& tile) {
    (*j)["Window"] = {
            {"client", tile.client},
            {"style", tile.style},
            {"xmin", tile.xLimits.first},
            {"xmax", tile.xLimits.second},
            {"ymin", tile.yLimits.first},
            {"ymax", tile.yLimits.second},
            {"primaryID", tile.id}
    };
}

void Message::InitClientSocket(const zmq::socket_t &toClient) {
    Message::client = &toClient;
}


void Message::SendToClient(json *j) {
    std::string jmsg_str = j->dump();
    zmq::message_t msg(jmsg_str.size());
    memcpy(msg.data(), jmsg_str.c_str(), jmsg_str.size());
    Message::client->send(msg, ZMQ_NOBLOCK);
}