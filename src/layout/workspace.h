#ifndef SLATEWM_WORKSPACE_H
#define SLATEWM_WORKSPACE_H

#include <X11/Xlib.h>

#include <unordered_set>
#include <string>

#include "tile.h"
#include "../slate.h"

class Slate;
class Tile;

class Workspace {
public:
    unsigned int focused_client;
    Tile* root;

    Workspace(Slate* wm);
    Workspace(Slate* wm, std::string name);
    ~Workspace();
    void removeClient(Slate* wm, Window w);
    void addClient(Slate* wm, Window w);

    static int default_count;
    static std::unordered_map<Window, std::string> clientLUT;

    static void switchTo(Slate* wm, std::string targetName);

private:
    std::string name;
    std::unordered_map<Window, Tile*> tileLUT;
    std::unordered_set<unsigned int> clients;


    static void hideWorkspace(Slate* wm);
    static void showWorkspace(Slate* wm, std::string targetName);

};


#endif //SLATEWM_WORKSPACE_H
