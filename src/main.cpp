#include <iostream>
#include <thread>

#include "slate.h"
#include "client_handler.h"

int main() {
    /* Entry point for slate */
    std::shared_ptr<Slate> wm = Slate::getInstance();
    if (!wm) {
        std::cout << "Failed to initialize slate" << std::endl;
        return -1;
    }

    std::thread T_Event(Slate::XEventLoopWrapper);
    std::thread T_Client(ClientHandler::Run);
    T_Event.join();
    T_Client.join();
    return 0;
}

