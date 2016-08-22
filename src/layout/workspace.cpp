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
    if (focusedTile->getPrimary() != nullptr) {
        tileLUT[focusedTile->getPrimary()->getClient().get()] = const_cast<Tile*>(focusedTile->getPrimary());
    }
}

void Workspace::removeClient(Window w) {
    clientLUT.erase(w);
    clients.erase(w);
    Tile* target = tileLUT[w];
    const Tile* parent = target->getParent();
    unsigned int width = target->getXLimits().second;
    unsigned int height = target->getYLimits().second;
    target->destroy();
    tileLUT.erase(w);
    if (target == root) {
        root = new Tile(width, height);
    }
    else if (parent != nullptr && parent->getClient()) {
        tileLUT[parent->getClient().get()] = const_cast<Tile*>(parent);
    }
}

Tile* Workspace::getRoot() const {
    return root;
}

void Workspace::setFocusedClient(unsigned int client) {
    focused_client = client;
}

