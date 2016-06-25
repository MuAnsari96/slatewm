#include <iostream>
#include "client_handler.h"
#include "message.h"
#include "layout/client.h"

void ClientHandler::Run(Slate* wm) {
    zmq::message_t reply;
    while (true) {
        wm->fromclient.recv(&reply);
        std::string msg(static_cast<char*>(reply.data()), reply.size()+1);
        msg[reply.size()] = '\0';
        json jmsg = json::parse(msg);
        int event = jmsg["Event"];

        switch(event) {
            case Kill_FOCUSED_CLIENT:
                XKillClient(wm->display, wm->state.focused_client);
                break;
            case HIDE_FOCUSED_CLIENT:
            std::cout << Client::clientID(wm->display, wm->state.focused_client) << std::endl;
                XUnmapWindow(wm->display, Client::clientID(wm->display, wm->state.focused_client));
                break;
            case UNHIDE_FOCUSED_CLIENT:
                XMapWindow(wm->display, Client::clientID(wm->display, wm->state.focused_client));
                break;
            default:
                break;
        }
    }
}

