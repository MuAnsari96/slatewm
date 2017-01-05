#ifndef SLATEWM_WORKSPACE_H
#define SLATEWM_WORKSPACE_H

#include <windows.h>

#include <unordered_set>
#include <unordered_map>
#include <string>

#include "tile.h"

typedef HWND Window;

class Workspace {
public:
    Workspace(unsigned int width, unsigned int height);
    Workspace(unsigned int width, unsigned int height, std::string name);
    ~Workspace();

    void addClient(Window w);
    void removeClient(Window w);

    Tile* getRoot() const;
    void setFocusedClient(HWND client);

    static int default_count;
    static std::unordered_map<Window, std::string> clientLUT;

    std::unordered_set<HWND> clients;

private:
    std::string name;
    std::unordered_map<Window, Tile*> tileLUT;

    Tile* root;
    HWND focused_client;
};


#endif //SLATEWM_WORKSPACE_H
