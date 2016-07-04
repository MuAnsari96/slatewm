#include <iostream>
#include <thread>
#include "slate.h"
#include <unordered_set>
#include <unordered_map>

int main() {
    std::shared_ptr<Slate> wm = Slate::getInstance();
    if (!wm) {
        std::cout << "Failed to initialize slate" << std::endl;
        return -1;
    }

    std::thread T_Event(Slate::XEventLoopWrapper);
    std::thread T_Client(Slate::ClientLoopWrapper);
    T_Event.join();
    T_Client.join();
    return 0;
}

