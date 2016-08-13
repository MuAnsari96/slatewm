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
    // TODO: Switch to python
    json msg;
    assert(first == nullptr && second == nullptr);
    if (!this->client) {
        this->client = client;
        msg["Event"] = "FillRootWindow";
        Message::AppendToMessage(&msg, *this);
        Message::SendToClient(&msg);

        return this;
    }

    msg["Event"] = "SplitWindow";
    first = new Tile(this, this->client);
    second = new Tile(this, client);


    this->client.reset();
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

void Tile::recalculateBoundaries() {
    // TODO: Switch to python
    if (!first || !second) {
        return;
    }
    if (styleType == HORIZONTAL) {
        first->xLimits = xLimits;
        first->yLimits = {yLimits.first, (yLimits.first + yLimits.second)/2};
        second->xLimits = xLimits;
        second->yLimits = {(yLimits.first + yLimits.second)/2, yLimits.second};
    }
    else if (styleType == VERTICAL) {
        first->xLimits = {xLimits.first, (xLimits.first + xLimits.second)/2};
        first->yLimits = yLimits;
        second->xLimits = {(xLimits.first + xLimits.second)/2, xLimits.second};
        second->yLimits = yLimits;
    }

    if (first->xLimits.second - first->xLimits.first >= first->yLimits.second - first->yLimits.first) {
        first->styleType = VERTICAL;
    }
    else {
        first->styleType = HORIZONTAL;
    }

    if (second->xLimits.second - second->xLimits.first >= second->yLimits.second - second->yLimits.first) {
        second->styleType = VERTICAL;
    }
    else {
        second->styleType = HORIZONTAL;
    }
    first->recalculateBoundaries();
    second->recalculateBoundaries();
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
        recalculateBoundaries();
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