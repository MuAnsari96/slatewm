#include <iostream>
#include "slate.h"

using namespace std;

int main() {
    std::unique_ptr<Slate> wm = Slate::getInstance();
    if (!wm) {
        std::cout << "Failed to initialize slate" << std::endl;
        return -1;
    }

    wm->XEventLoop();
    return 0;
}