#include <iostream>
#include <X11/Xlib.h>
#include "workspace.h"
#include "client.h"
#include <boost/optional/optional_io.hpp>

int Workspace::default_count = 1;
std::unordered_map<Window, std::string> Workspace::clientLUT;

Workspace::Workspace(Slate* wm) {
    name = std::to_string(default_count);
    default_count++;
    focused_client = -1;

    XWindowAttributes attr;
    XGetWindowAttributes(wm->display, wm->root, &attr);
    root = new Tile(attr.width, attr.height);
}

Workspace::Workspace(Slate* wm, std::string name) {
    this->name = name;
    focused_client = -1;
    XWindowAttributes attr;
    XGetWindowAttributes(wm->display, wm->root, &attr);
    root = new Tile(attr.width, attr.height);
}

Workspace::~Workspace() {
    delete root;
}
using namespace std;
void Workspace::addClient(Slate* wm, Window w) {
    Window top = Client::clientID(wm->display, w);
    if (clients.count(top) > 0) {
        return;
    }
    clients.insert(top);
    clientLUT[w] = name;
    Window topFocused = Client::clientID(wm->display, wm->state.focused_client);
    Tile* focusedTile = root;
    if (tileLUT.count(topFocused) > 0) {
        focusedTile = tileLUT[topFocused];
        cout << "has!" << endl;
    }
    tileLUT[w] = focusedTile->assignClient(wm, w);
    if (focusedTile->first != nullptr) {
        std::cout << focusedTile->first->client << std::endl;
        tileLUT[focusedTile->first->client.get()] = focusedTile->first;
    }
}

void Workspace::removeClient(Slate* wm, Window w) {
    clients.erase(w);
    clientLUT.erase(w);
    Tile* target = tileLUT[w];
    Tile* parent = target->parent;
    delete target;
    tileLUT.erase(w);
    if (target == root) {
        XWindowAttributes attr;
        XGetWindowAttributes(wm->display, wm->root, &attr);
        root = new Tile(attr.width, attr.height);
    }
    else if (parent != nullptr) {
        tileLUT[parent->client.get()] = parent;
    }
    root->drawTile(wm);
}

void Workspace::switchTo(Slate *wm, std::string targetName) {
    if (wm->state.workspaceID == targetName) {
        return;
    }
    hideWorkspace(wm);
    showWorkspace(wm, targetName);
}

void Workspace::hideWorkspace(Slate *wm) {
    Workspace *curr = wm->state.workspaces[wm->state.workspaceID];
    std::unordered_set<Window> clients = curr->clients;
    for (Window client: clients) {
        XUnmapWindow(wm->display, client);
    }
}

void Workspace::showWorkspace(Slate *wm, std::string targetName) {
    if (wm->state.workspaces.count(targetName) == 0) {
        wm->state.workspaces[targetName] = new Workspace(wm, targetName);
    }
    Workspace *target = wm->state.workspaces[targetName];
    std::unordered_set<Window> clients = target->clients;
    for (Window client: clients) {
        XMapWindow(wm->display, client);
    }
    target->root->drawTile(wm);
    wm->state.workspaceID = targetName;
}