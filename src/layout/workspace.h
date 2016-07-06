#ifndef SLATEWM_WORKSPACE_H
#define SLATEWM_WORKSPACE_H

#include <X11/Xlib.h>

#include <unordered_set>
#include <unordered_map>
#include <string>

#include "tile.h"

class Workspace {
public:
    unsigned int focused_client;
    Tile* root;

    Workspace(unsigned int width, unsigned int height);
    Workspace(unsigned int width, unsigned int height, std::string name);
    ~Workspace();
    void addClient(Display* display, Window w);
    void removeClient(Window w);

    static int default_count;
    static std::unordered_map<Window, std::string> clientLUT;

    std::unordered_set<unsigned int> clients;

private:
    std::string name;
    std::unordered_map<Window, Tile*> tileLUT;
};


#endif //SLATEWM_WORKSPACE_H
