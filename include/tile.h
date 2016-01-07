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
        tileType type;
        Tile(tileType _type) : type(_type) {};
        static bool areEqual(const Tile &a, const Tile &b);
        virtual ~Tile() {};
        virtual std::string get_val() const = 0;
        virtual std::string get_className() const = 0;
        static std::vector<Tile*>createTheTiles();
};

struct SortTiles { // like a Comparator in Java
    bool operator()(const Tile *a, const Tile *b) const { return a->get_val() < b->get_val(); };
};

class SuitTile : public Tile {
        std::string className;
    public:
        int suit;
        SuitTile(std::string name, int suitNumber) : Tile(SUIT), className(name), suit(suitNumber) {};
        friend bool const operator<(const SuitTile &a, const SuitTile &b);
        friend bool const operator>(const SuitTile &a, const SuitTile &b);
        static bool inSequence(std::vector<SuitTile*> tiles); // returns whether the tiles' suit numbers are consecutive terms
        std::string get_val() const;
        std::string get_className() const;
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
    std::string attribute, className;
    public:
        AttributeTile(std::string name, tileType _tileType, std::string attribute_type) : Tile(_tileType), attribute(attribute_type), className(name) {};
        std::string get_val() const;
        std::string get_className() const;
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
