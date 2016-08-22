#ifndef SLATEWM_TILE_H
#define SLATEWM_TILE_H

#include <X11/Xlib.h>

#include <boost/optional/optional_io.hpp>
#include <utility>
#include <unordered_map>

typedef std::pair<unsigned int, unsigned int> tuple;
class Slate;

enum StyleType {
    TILE = 0,
    STACK = 1,
    FLOAT = 2
};

class Tile {
public:
    Tile();
    Tile(Tile* parent, boost::optional<Window> client);
    Tile(int xMax, int yMax);
    Tile(tuple xLimits, tuple yLimits, Tile* parent, boost::optional<Window> client);
    Tile(tuple xLimits, tuple yLimits, Tile* parent,
         boost::optional<Window> client, StyleType styleType);

    ~Tile();

    void destroy();
    void deleteChild(Tile* child);
    Tile* assignClient(Window client);

    void drawTile(Display* display);
    void recalculateBoundaries(bool isRoot);

    friend std::ostream& operator<< (std::ostream& out, const Tile& tile);

    const Tile* getPrimary() const;
    const Tile* getSecondary() const;
    const Tile* getParent() const;
    const boost::optional<Window> getClient() const;
    const tuple& getXLimits() const;
    const tuple& getYLimits() const;
    const std::string& getStyle() const;
    const StyleType& getStyleType() const;
    unsigned int getID() const;


    static unsigned int nextIndex;
    static std::unordered_map<unsigned int, Tile*> tileLUT;

    static Tile* restyleTile(unsigned int id, tuple xLimits, tuple yLimits, StyleType styleType, std::string style, bool root);

private:
    Tile* first;
    Tile* second;
    Tile* parent;

    boost::optional<Window> client;

    tuple xLimits;
    tuple yLimits;

    std::string style;
    StyleType styleType;
    unsigned int id;
};


#endif //SLATEWM_TILE_H
