#include <iostream>
#include "client_handler.h"

void ClientHandler::Run(Slate* wm) {
    zmq::message_t reply;
    while (true) {
        wm->fromclient.recv(&reply);
        std::cout << std::string(static_cast<char*>(reply.data())) << std::endl;
    }
}

