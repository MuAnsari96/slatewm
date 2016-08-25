#include "tile.h"

#include "../util/message.h"

unsigned int Tile::nextIndex = 1;
std::unordered_map<unsigned int, Tile*> Tile::tileLUT;


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

void Tile::deleteChild(Tile *child) {
    /* When a client is deleted, the "tree" that represents the tiled clients must be reordered, and boundaries
     * recalculated. This functions handles this case.
     */
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
        recalculateBoundaries(parent == nullptr);
    }

    prop->parent = nullptr;
    prop->destroy();
}

Tile* Tile::assignClient(Window client) {
    /* This function is called whenever a client is opened by the user-- it is then either mapped to an uninited tile,
     * or the client that it was spawned in is split to show the new client.
     */
    json msg;
    assert(first == nullptr && second == nullptr);
    if (!this->client) {
        this->client = client;
        msg["Event"] = Message::ToClient::GET_ROOT_WINDOW;
        Message::AppendToMessage(&msg, *this);
        Message::SendToClient(&msg);

        return this;
    }

    first = new Tile(this, this->client);
    second = new Tile(this, client);
    this->client.reset();
    msg["Event"] = Message::ToClient::GET_CHILD_WINDOWS;
    Message::AppendToMessage(&msg, *this, *first, *second);
    Message::SendToClient(&msg);

    return second;
}

void Tile::drawTile(Display* display) {
    /* Draws a tile and all of its subchildren to the screen */
    if (client) {
        // Even though the XLib docs SAY NOTHING AT ALL ABOUT THIS, windows need nonzero dimensions
        if (xLimits.second - xLimits.first == 0 || yLimits.second - yLimits.first == 0){
            return;
        };

        XMoveResizeWindow(display, client.get(), xLimits.first, yLimits.first,
                          xLimits.second - xLimits.first, yLimits.second - yLimits.first);
    }
    if (first != nullptr && second != nullptr) {
        first->drawTile(display);
        second->drawTile(display);
    }
}

void Tile::recalculateBoundaries(bool isRoot) {
    if (!first || !second) {
        return;
    }

    json msg;
    if (isRoot) {
        msg["Event"] = Message::ToClient::GET_ROOT_WINDOW;
        Message::AppendToMessage(&msg, *this);
        Message::SendToClient(&msg);
        return;
    }

    msg["Event"] = Message::ToClient::GET_CHILD_WINDOWS;
    Message::AppendToMessage(&msg, *this, *first, *second);
    Message::SendToClient(&msg);
}

Tile* Tile::restyleTile(unsigned int id, tuple xLimits, tuple yLimits,
                        StyleType styleType, std::string style, bool root) {
    /* Updates the style of a window (as defined in slate's client modules). This is called whenever
     * the client emits an even that warrants a change in the displayed windows
     */
    Tile* tile = tileLUT[id];
    tile->style = style;
    tile->styleType = styleType;

    if (root) {
        switch(styleType) {
            case StyleType::TILE:
            case StyleType::STACK:
                return tile;
            case StyleType::FLOAT:
                break;
        }
    }

    tile->xLimits = xLimits;
    tile->yLimits = yLimits;
    return tile;
}

const Tile* Tile::getPrimary() const {
    return first;
}

const Tile* Tile::getSecondary() const {
    return second;
}

const Tile* Tile::getParent() const {
    return parent;
}

const boost::optional<Window> Tile::getClient() const {
    return client;
}

const tuple& Tile::getXLimits() const {
    return xLimits;
}

const tuple& Tile::getYLimits() const {
    return yLimits;
}

const std::string& Tile::getStyle() const {
    return style;
}

const StyleType& Tile::getStyleType() const {
    return styleType;
}

unsigned int Tile::getID() const {
    return id;
}

std::ostream& operator<< (std::ostream& out, const Tile& tile) {
    out << "Tile(x: " << tile.xLimits.first << ", " << tile.xLimits.second << " | y: " << tile.yLimits.first
        << ", " << tile.yLimits.second << " | client: " << tile.client << ")" << std::endl;
}
