#ifndef SLATEWM_TILE_H
#define SLATEWM_TILE_H

#include <X11/Xlib.h>

#include <boost/optional/optional_io.hpp>
#include <utility>
#include <bits/unordered_map.h>
#include "../../lib/json.h"

using json = nlohmann::json;
typedef std::pair<unsigned int, unsigned int> tuple;
class Slate;

enum SplitType {
    HORIZONTAL,
    VERTICAL
};

class Tile {
public:
    Tile();
    Tile(int xMax, int yMax);
    Tile(tuple xLimits, tuple yLimits, Tile* parent, boost::optional<Window> client);
    Tile(tuple xLimits, tuple yLimits, Tile* parent,
         boost::optional<Window> client, SplitType splitType);

    ~Tile();

    Tile* assignClient(Window client);

    void drawTile(Display* display);
    void recalculateBoundaries();
    void deleteChild(Tile* child);
    void destroy();
    void printHier(int level);
    void addToJson(json* j);

    friend std::ostream& operator<< (std::ostream& out, const Tile& tile);



    SplitType splitType;
    Tile* first;
    Tile* second;
    Tile* parent;

    boost::optional<Window> client;

    tuple xLimits;
    tuple yLimits;

    std::string style;
    unsigned int id;

    static unsigned int nextIndex;
    static std::unordered_map<unsigned int, Tile*> tileLUT;

    static void restyleTile(unsigned int id, tuple xLimits, tuple yLimits, std::string style);
};


#endif //SLATEWM_TILE_H
