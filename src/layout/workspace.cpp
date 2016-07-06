#include "workspace.h"

#include <iostream>

#include "client.h"

int Workspace::default_count = 1;
std::unordered_map<Window, std::string> Workspace::clientLUT;

Workspace::Workspace(unsigned int width, unsigned int height) {
    name = std::to_string(default_count);
    default_count++;
    focused_client = 0;

    root = new Tile(width, height);
}

Workspace::Workspace(unsigned int width, unsigned int height, std::string name) {
    this->name = name;
    focused_client = 0;
    root = new Tile(width, height);
}

Workspace::~Workspace() {
    root->destroy();
}

void Workspace::addClient(Display* display, Window w) {
    Window top = Client::clientID(display, w);
    if (clients.count(top) > 0) {
        return;
    }
    clients.emplace(top);
    clientLUT[w] = name;
    Window topFocused = Client::clientID(display, focused_client);
    Tile* focusedTile = root;
    if (tileLUT.count(topFocused) > 0) {
        focusedTile = tileLUT[topFocused];
    }
    tileLUT[w] = focusedTile->assignClient(w);
    if (focusedTile->first != nullptr) {
        tileLUT[focusedTile->first->client.get()] = focusedTile->first;
    }
}

void Workspace::removeClient(Window w) {
    clientLUT.erase(w);
    clients.erase(w);
    Tile* target = tileLUT[w];
    Tile* parent = target->parent;
    unsigned int width = target->xLimits.second;
    unsigned int height = target->yLimits.second;
    target->destroy();
    tileLUT.erase(w);
    if (target == root) {
        root = new Tile(width, height);
    }
    else if (parent != nullptr && parent->client) {
        tileLUT[parent->client.get()] = parent;
    }
}

