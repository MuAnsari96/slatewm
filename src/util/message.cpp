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
            {"style", tile.getStyle()},
            {"styleType", tile.getStyleType()},
            {"xmin", tile.getXLimits().first},
            {"xmax", tile.getXLimits().second},
            {"ymin", tile.getYLimits().first},
            {"ymax", tile.getYLimits().second},
            {"primaryID", tile.getID()}
    };
}

void Message::AppendToMessage(json *j, const Tile &toSplit, const Tile &primary, const Tile &secondary) {
    (*j)["Window"] = {
            {"style", toSplit.getStyle()},
            {"styleType", toSplit.getStyleType()},
            {"xmin", toSplit.getXLimits().first},
            {"xmax", toSplit.getXLimits().second},
            {"ymin", toSplit.getYLimits().first},
            {"ymax", toSplit.getYLimits().second},
            {"primaryID", primary.getID()},
            {"secondaryID", secondary.getID()}
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