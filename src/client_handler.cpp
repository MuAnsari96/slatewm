#include <iostream>
#include "client_handler.h"

void ClientHandler::Run(Slate* wm) {
    zmq::message_t reply;
    while (true) {
        wm->fromclient.recv(&reply);
        std::string msg(static_cast<char*>(reply.data()), reply.size()+1);
        msg[reply.size()] = '\0';
        std::cout << msg << std::endl;
    }
}

