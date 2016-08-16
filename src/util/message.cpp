#include "message.h"

namespace Message {
    zmq::socket_t* client;
}

void Message::PopulateMessage(json *j, const slate_state_t &state, const XEvent &e) {
    (*j)["Event"] = ToClient::DEFAULT;
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
            {"style", tile.style},
            {"styleType", tile.styleType},
            {"xmin", tile.xLimits.first},
            {"xmax", tile.xLimits.second},
            {"ymin", tile.yLimits.first},
            {"ymax", tile.yLimits.second},
            {"primaryID", tile.id}
    };
}

void Message::AppendToMessage(json *j, const Tile &toSplit, const Tile &primary, const Tile &secondary) {
    (*j)["Window"] = {
            {"style", toSplit.style},
            {"styleType", toSplit.styleType},
            {"xmin", toSplit.xLimits.first},
            {"xmax", toSplit.xLimits.second},
            {"ymin", toSplit.yLimits.first},
            {"ymax", toSplit.yLimits.second},
            {"primaryID", primary.id},
            {"secondaryID", secondary.id}
    };
}

void Message::InitClientSocket(zmq::socket_t* toClient) {
    client = toClient;
}


void Message::SendToClient(json *j) {
    std::string jmsg_str = j->dump();
    zmq::message_t msg(jmsg_str.size());
    memcpy(msg.data(), jmsg_str.c_str(), jmsg_str.size());
    Message::client->send(msg, ZMQ_NOBLOCK);
}