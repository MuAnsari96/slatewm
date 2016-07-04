#include <iostream>
#include "tile.h"

Tile::Tile() :
    xLimits{0, 0}, yLimits{0, 0}, parent{nullptr},
    first{nullptr}, second{nullptr}, splitType{VERTICAL} {}

Tile::Tile(unsigned int xMax, unsigned int yMax) :
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
    assert(client);
    if (parent != nullptr) {
        parent->deleteChild(this);
    }
}

Tile* Tile::assignClient(Slate* wm, Window client) {
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

void Tile::drawTile(Slate* wm) {
    if (client) {
        XMoveResizeWindow(wm->display, client.get(), xLimits.first, yLimits.first,
                          xLimits.second - xLimits.first, yLimits.second - yLimits.first);
    }
    if (first != nullptr && second != nullptr) {
        first->drawTile(wm);
        second->drawTile(wm);
    }
}

void Tile::recalculateBoundaries() {
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

    first = nullptr;
    second = nullptr;
    client = prop->client;

    prop->parent = nullptr;
    delete prop;
}

void Tile::killUpdate(Slate *wm, Tile *tile) {
    Tile* parent= tile->parent;
    XKillClient(wm->display, tile->client.get());
    delete tile;
    parent->drawTile(wm);
}

std::ostream& operator<< (std::ostream out, const Tile& tile) {
    out << "Tile(x: " << tile.xLimits.first << ", " << tile.xLimits.second << " | y: " << tile.yLimits.first
        << ", " << tile.yLimits.second << " | client: " << tile.client << ")" << std::endl;
};