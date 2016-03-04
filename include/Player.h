#ifndef PLAYER_H
#define PLAYER_H
#include "HandEvaluator.h"
#include "Tile.h"
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <utility>
#define FULL_HAND_SIZE 13
#define WINDS \
    X(EAST)\
    X(SOUTH)\
    X(WEST)\
    X(NORTH)

#define X(a) a,
enum wind { WINDS };// order prevailing winds rotate in
#undef X

#define X(a) #a,
static std::string wind_strings[] = { WINDS };
#undef X

enum class Declaration;
typedef std::shared_ptr<Tile> player_tile_t;
typedef std::vector<player_tile_t> player_tiles_t;
class Player
{
    public:
        wind _wind;
        int score;
		
        player_tiles_t hand, discards, bonuses;
        std::vector<std::pair<MeldType, player_tiles_t>> melds;
        static std::map<Declaration, MeldType> declarationToMeld;
        Player();
        friend bool const operator==(const Player &a, const Player &b);
        friend bool const operator!=(const Player &a, const Player &b);
        virtual void takeTile(tile_t tile);
        virtual Tile *discardTile(int selected_index=0);
        virtual void makeMeld(player_tile_t discardedTile=nullptr);
        bool hasHandSize(int _size);
        void sortHand();
        void determineOptions(Tile *discardedTile=nullptr);
        std::pair<Declaration, int> getDeclaration();
        virtual void setDeclaration(std::pair<Declaration, int> declaration);
        std::map<MeldType, std::vector<meld>> getOptions();
        virtual ~Player();
    protected:
        HandEvaluator *handEvaluator;
        std::map<MeldType, std::vector<meld>> options;
        std::pair<Declaration, int> curr_declaration;
};

#define DECLARATIONS \
    X(SMALL_MELDED_KANG) \
    X(CONCEALED_KANG) \
    X(MELDED_PENG) \
    X(MELDED_CHI) \
    X(MELDED_KANG) \
    X(MAHJONG)

#define X(a) a,
enum class Declaration {
    NONE,
    DECLARATIONS
};
#undef X

#endif // PLAYER_H
