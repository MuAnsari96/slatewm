#ifndef SLATEWM_TILE_H
#define SLATEWM_TILE_H

#include <utility>
#include "boost/optional.hpp"
#include <boost/optional/optional_io.hpp>
#include "../slate.h"
#include <X11/Xlib.h>

typedef std::pair<unsigned int, unsigned int> tuple;
class Slate;

enum SplitType {
    HORIZONTAL,
    VERTICAL
};

class Tile {
public:
    Tile();
    Tile(unsigned int xMax, unsigned int yMax);
    Tile(tuple xLimits, tuple yLimits, Tile* parent, boost::optional<Window> client);
    Tile(tuple xLimits, tuple yLimits, Tile* parent,
         boost::optional<Window> client, SplitType splitType);

    ~Tile();

    Tile* assignClient(Slate* wm, Window client);

    void drawTile(Slate* wm);
    void recalculateBoundaries();
    void deleteChild(Tile* child);
    void destroy();
    void printHier(int level);

    friend std::ostream& operator<< (std::ostream& out, const Tile& tile);

    static void killUpdate(Slate* wm, Tile *tile);


    SplitType splitType;
    Tile* first;
    Tile* second;
    Tile* parent;

    boost::optional<Window> client;

    tuple xLimits;
    tuple yLimits;
};


#endif //SLATEWM_TILE_H
