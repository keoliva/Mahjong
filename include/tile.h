#ifndef TILE_H_INCLUDED
#define TILE_H_INCLUDED

#include <string>
#include <vector>

enum tileType {
    SUIT, // BambooTile, CharacterTile, CircleTile
    HONOR, // DragonTile, WindTile
    BONUS, // FlowerTile, SeasonTile,
};

class Tile {
    public:
        Tile(tileType _type) : type(_type) {};
        virtual ~Tile() {};
        virtual std::string get_val() const = 0;
        static std::vector<Tile*>createTheTiles();
    protected:
        tileType type;
};

struct SortTiles { // like a Comparator in Java
    bool operator()(const Tile *a, const Tile *b) const { return a->get_val() < b->get_val(); };
};

class SuitTile : public Tile {
    public:
        int suit;
        std::string className;
        SuitTile(std::string name, int suitNumber) : Tile(SUIT), suit(suitNumber), className(name) {};
        std::string get_val() const;
};

class BambooTile : public SuitTile {
    public:
        BambooTile(int suitNumber) : SuitTile("BambooTile", suitNumber) {};
};
class CharacterTile : public SuitTile {
    public:
        CharacterTile(int suitNumber) : SuitTile("CharacterTile", suitNumber) {};
};
class CircleTile : public SuitTile {
    public:
        CircleTile(int suitNumber) : SuitTile("CircleTile", suitNumber) {};
};

class AttributeTile : public Tile {
    public:
        std::string attribute, className;
        AttributeTile(std::string name, tileType _tileType, std::string attribute_type) : Tile(_tileType), attribute(attribute_type), className(name) {};
        std::string get_val() const;
};

class DragonTile : public AttributeTile { // dragon types: GREEN, RED, WHITE
    public:
        DragonTile(std::string dragonType) : AttributeTile("DragonTile", HONOR, dragonType) {};
};
class WindTile : public AttributeTile { // wind types: NORTH, EAST, WEST, SOUTH
    public:
        WindTile(std::string windType) : AttributeTile("WindTile", HONOR, windType) {};
};

class FlowerTile : public AttributeTile { // flower types: PLUMB, ORCHID, CHRYSANTHEMUM, BAMBOO
    public:
        FlowerTile(std::string flowerType) : AttributeTile("FlowerTile", BONUS, flowerType) {};
};
class SeasonTile : public AttributeTile { // season types: SPRING, SUMMER, AUTUMN, WINTER
    public:
        SeasonTile(std::string seasonType) : AttributeTile("SeasonTile", BONUS, seasonType) {};
};
#endif // TILE_H_INCLUDED
