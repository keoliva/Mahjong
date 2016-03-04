#ifndef TILE_H_INCLUDED
#define TILE_H_INCLUDED

#include <memory>
#include <string>
#include <vector>

enum tileType {
	NIL_TYPE,
    SUIT, // BambooTile, CharacterTile, CircleTile
    HONOR, // DragonTile, WindTile
    BONUS, // FlowerTile, SeasonTile,
};
class Tile;
typedef std::unique_ptr<Tile> tile_t;
typedef std::vector<tile_t> tiles_t;
class Tile {
    public:
        std::string className;
        tileType type;
        Tile(std::string _name, tileType _type) : className(_name), type(_type) {};
		virtual bool isNull() { return false; };
        std::string get_className() const {return className; };
        static bool areEqual(const Tile &a, const Tile &b);
		virtual Tile *clone() const = 0;
        virtual ~Tile() {};
        virtual std::string get_val() const = 0;
        static void createTheTiles(tiles_t &pile);
};


template <typename T>
struct SortTiles { // like a Comparator in Java
    bool operator()(const T &a, const T &b) const {
        return a->get_val() < b->get_val();
    };
};

class NullTile : public Tile {
  public:
	NullTile() : Tile("", NIL_TYPE) {};
	bool isNull() { return true; };
	std::string get_val() const { return "NullTile"; };
	Tile *clone() const { return new NullTile(); };
};

class SuitTile : public Tile {
    public:
        int suit;
        SuitTile(std::string name, int suitNumber) : Tile(name, SUIT), suit(suitNumber) {};
        friend bool const operator<(const SuitTile &a, const SuitTile &b);
        friend bool const operator>(const SuitTile &a, const SuitTile &b);
        static bool inSequence(std::vector<SuitTile*> tiles); // returns whether the tiles' suit numbers are consecutive terms
        std::string get_val() const;
		Tile *clone() const;
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
		std::string attribute;
        AttributeTile(std::string name, tileType _tileType, std::string attribute_type) : Tile(name, _tileType),
                                                                                        attribute(attribute_type) {};
        std::string get_val() const;
		Tile *clone() const;
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
