#include <iostream>
#include "client_handler.h"
#include "message.h"



void ClientHandler::Run(Slate* wm) {
    zmq::message_t reply;
    while (true) {
        wm->fromclient.recv(&reply);
        std::string msg(static_cast<char*>(reply.data()), reply.size()+1);
        msg[reply.size()] = '\0';
        json jmsg = json::parse(msg);

        if (jmsg["Event"] == "CloseFocusedClient")
            XKillClient(wm->display, wm->state.focused_client);
        else
            continue;
    }
}

