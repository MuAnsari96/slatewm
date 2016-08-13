#include "tile.h"
#include "../util/message.h"

unsigned int Tile::nextIndex = 1;

Tile::Tile() :
        xLimits{0, 0}, yLimits{0, 0}, parent{nullptr}, id{nextIndex++},
        first{nullptr}, second{nullptr}, styleType{TILE}, style{""} {
    tileLUT[id] = this;
}

Tile::Tile(Tile* parent, boost::optional<Window> client) :
        xLimits{0, 0}, yLimits{0, 0}, parent{parent}, id{nextIndex++},
        client{client}, first{nullptr}, second{nullptr}, styleType{TILE}, style{""} {
    tileLUT[id] = this;
}

Tile::Tile(int xMax, int yMax) :
        xLimits{0, xMax}, yLimits{0, yMax}, parent{nullptr}, id{nextIndex++},
        first{nullptr}, second{nullptr}, styleType{TILE}, style{""} {
    tileLUT[id] = this;
}

Tile::Tile(tuple xLimits, tuple yLimits, Tile* parent, boost::optional<Window> client) :
    xLimits{xLimits}, yLimits{yLimits}, parent{parent}, id{nextIndex++},
    client{client}, first{nullptr}, second{nullptr}, styleType{TILE}, style{""} {
    tileLUT[id] = this;
}

Tile::Tile(tuple xLimits, tuple yLimits, Tile* parent, boost::optional<Window> client, StyleType styleType) :
        xLimits{xLimits}, yLimits{yLimits}, parent{parent}, id{nextIndex++},
        client{client}, first{nullptr}, second{nullptr}, styleType{styleType}, style{""} {
    tileLUT[id] = this;
}

Tile::~Tile() {
    tileLUT.erase(id);
}

void Tile::destroy() {
    if (parent != nullptr) {
        parent->deleteChild(this);
    }
    delete this;
}

Tile* Tile::assignClient(Window client) {
    json msg;
    assert(first == nullptr && second == nullptr);
    if (!this->client) {
        this->client = client;
        msg["Event"] = Message::ToClient::FILL_ROOT_WINDOW;
        Message::AppendToMessage(&msg, *this);
        Message::SendToClient(&msg);

        return this;
    }

    first = new Tile(this, this->client);
    second = new Tile(this, client);
    this->client.reset();
    msg["Event"] = Message::ToClient::SPLIT_WINDOW;
    Message::AppendToMessage(&msg, *this, *first, *second);
    Message::SendToClient(&msg);

    return second;
}

void Tile::drawTile(Display* display) {
    if (client) {
        XMoveResizeWindow(display, client.get(), xLimits.first, yLimits.first,
                          xLimits.second - xLimits.first, yLimits.second - yLimits.first);
    }
    if (first != nullptr && second != nullptr) {
        first->drawTile(display);
        second->drawTile(display);
    }
}

void Tile::recalculateBoundaries(bool isRoot=false) {
    if (!first || !second) {
        return;
    }

    json msg;
    if (isRoot) {
        msg["Event"] = Message::ToClient::RECALCULATE_ROOT_BOUNDARY;
        Message::AppendToMessage(&msg, *this);
        Message::SendToClient(&msg);
        return;
    }

    msg["Event"] = Message::ToClient::RECALCULATE_BOUNDARIES;
    Message::AppendToMessage(&msg, *this, *first, *second);
    Message::SendToClient(&msg);
}

void Tile::deleteChild(Tile *child) {
    Tile *prop;
    if (first == child) {
        prop = second;
    }
    else if (second == child) {
        prop = first;
    }

    if (prop->client) {
        first = nullptr;
        second = nullptr;
        client = prop->client;
    }
    else {
        first = prop->first;
        second = prop->second;
        first->parent = this;
        second->parent = this;
        std::cout << *this << std::endl;
        recalculateBoundaries(parent == nullptr);
    }

    prop->parent = nullptr;
    prop->destroy();
}

void Tile::printHier(int level) {
    if (level == 0) {
        std::cout << "------------------------------------------" << std::endl;
    }
    for (int i = 0; i < level; i++) {
        std::cout << "  ";
    }
    std::cout << *this << std::endl;
    if (first && second) {
        first->printHier(level+1);
        second->printHier(level+1);
    }
}

std::ostream& operator<< (std::ostream& out, const Tile& tile) {
    out << "Tile(x: " << tile.xLimits.first << ", " << tile.xLimits.second << " | y: " << tile.yLimits.first
        << ", " << tile.yLimits.second << " | client: " << tile.client << ")" << std::endl;
}

void Tile::restyleTile(unsigned int id, tuple xLimits, tuple yLimits, std::string style) {
    Tile* tile = tileLUT[id];

}