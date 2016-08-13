#include "client_handler.h"

#include <iostream>

#include "util/message.h"
#include "layout/client.h"

void ClientHandler::Run() {
    std::shared_ptr<Slate> wm = Slate::getInstance();
    zmq::message_t reply;
    while (true) {
        wm->fromclient.recv(&reply);
        std::string msg(static_cast<char*>(reply.data()), reply.size()+1);
        msg[reply.size()] = '\0';
        json jmsg = json::parse(msg);
        int event = jmsg["Event"];

        switch(event) {
            case Message::FromClient::Kill_FOCUSED_CLIENT:
                XKillClient(wm->display, wm->state.focused_client);
                break;
            case Message::FromClient::HIDE_FOCUSED_CLIENT:
                XUnmapWindow(wm->display, Client::clientID(wm->display, wm->state.focused_client));
                break;
            case Message::FromClient::UNHIDE_FOCUSED_CLIENT:
                XMapWindow(wm->display, Client::clientID(wm->display, wm->state.focused_client));
                break;
            case Message::FromClient::SWITCH_WORKSPACE:
                wm->switchToWorkspace(jmsg["Workspace"]);
                break;
            default:
                break;
        }
    }
}

