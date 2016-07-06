#include "tile.h"

#include <iostream>

Tile::Tile() :
    xLimits{0, 0}, yLimits{0, 0}, parent{nullptr},
    first{nullptr}, second{nullptr}, splitType{VERTICAL} {}

Tile::Tile(int xMax, int yMax) :
        xLimits{0, xMax}, yLimits{0, yMax}, parent{nullptr},
        first{nullptr}, second{nullptr}, splitType{VERTICAL} {}

Tile::Tile(tuple xLimits, tuple yLimits, Tile* parent, boost::optional<Window> client) :
    xLimits{xLimits}, yLimits{yLimits}, parent{parent},
    client{client}, first{nullptr}, second{nullptr} {
    if (xLimits.second - xLimits.first >= yLimits.second - yLimits.first) {
        splitType = VERTICAL;
    }
    else {
        splitType = HORIZONTAL;
    }
}

Tile::Tile(tuple xLimits, tuple yLimits, Tile* parent, boost::optional<Window> client, SplitType splitType) :
        xLimits{xLimits}, yLimits{yLimits}, parent{parent},
        client{client}, first{nullptr}, second{nullptr}, splitType{splitType} {}

Tile::~Tile() {
}

void Tile::destroy() {
    if (parent != nullptr) {
        parent->deleteChild(this);
    }
    delete this;
}

Tile* Tile::assignClient(Window client) {
    assert(first == nullptr && second == nullptr);
    if (!this->client) {
        this->client = client;
        return this;
    }
    if (splitType == HORIZONTAL) {
        first = new Tile(xLimits, {yLimits.first, (yLimits.first + yLimits.second)/2},
                         this, this->client);
        second = new Tile(xLimits, {(yLimits.first + yLimits.second)/2, yLimits.second},
                         this, client);

    }
    else if (splitType == VERTICAL) {
        first = new Tile({xLimits.first, (xLimits.first + xLimits.second)/2}, yLimits,
                         this, this->client);
        second = new Tile({(xLimits.first + xLimits.second)/2, xLimits.second}, yLimits,
                          this, client);
    }
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
    if (!first || !second) {
        return;
    }
    if (splitType == HORIZONTAL) {
        first->xLimits = xLimits;
        first->yLimits = {yLimits.first, (yLimits.first + yLimits.second)/2};
        second->xLimits = xLimits;
        second->yLimits = {(yLimits.first + yLimits.second)/2, yLimits.second};
    }
    else if (splitType == VERTICAL) {
        first->xLimits = {xLimits.first, (xLimits.first + xLimits.second)/2};
        first->yLimits = yLimits;
        second->xLimits = {(xLimits.first + xLimits.second)/2, xLimits.second};
        second->yLimits = yLimits;
    }

    if (first->xLimits.second - first->xLimits.first >= first->yLimits.second - first->yLimits.first) {
        first->splitType = VERTICAL;
    }
    else {
        first->splitType = HORIZONTAL;
    }

    if (second->xLimits.second - second->xLimits.first >= second->yLimits.second - second->yLimits.first) {
        second->splitType = VERTICAL;
    }
    else {
        second->splitType = HORIZONTAL;
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


