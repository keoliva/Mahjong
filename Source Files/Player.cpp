#include "include/Player.h"
#include "include/Tile.h"
#include <cassert>
#include <algorithm>
using namespace std;
std::map<Declaration, MeldType> Player::declarationToMeld = {
    {Declaration::SMALL_MELDED_KANG, MeldType::SMALL_MELDED_KANG},
    {Declaration::CONCEALED_KANG, MeldType::CONCEALED_KANG},
    {Declaration::MELDED_PENG, MeldType::PENG},
    {Declaration::MELDED_CHI, MeldType::CHI},
    {Declaration::MELDED_KANG, MeldType::KANG}
};

Player::Player() {
    score = 0;
    curr_declaration = std::make_pair(Declaration::NONE, -1);
    handEvaluator = new HandEvaluator();
}
bool const operator==(const Player &a, const Player &b) {
    return a._wind == b._wind;
}
bool const operator!=(const Player &a, const Player &b) {
    return !(a == b);
}
Player::~Player() {
    std::cout << "deleting player." << std::endl;
    delete handEvaluator;
}
void Player::determineOptions(Tile *discardedTile) {
    options.clear();
	vector<Tile*> _hand;
	player_tiles_t::iterator it = hand.begin();
	for (; it != hand.end(); it++)
		_hand.push_back((*it).get());
    if (discardedTile && !discardedTile->isNull()) {
        options[PENG] = handEvaluator->canDeclareMeldedPeng(_hand, discardedTile);
        options[KANG] = handEvaluator->canDeclareBigMeldedKang(_hand, discardedTile);
        options[CHI] = handEvaluator->canDeclareMeldedChi(_hand, discardedTile);
    } else {
		vector<pair<MeldType, vector<Tile*>>> _melds;
		for (pair<MeldType, player_tiles_t> meld : melds) {
			vector<Tile*> tilesInMeld;
			player_tiles_t::iterator it = meld.second.begin();
			for (; it != meld.second.end(); it++) {
				tilesInMeld.push_back((*it).get());
			}
			_melds.push_back(make_pair(meld.first, tilesInMeld));
		}
        options[SMALL_MELDED_KANG] = handEvaluator->canDeclareSmallMeldedKang(_melds, _hand);
        options[CONCEALED_KANG] = handEvaluator->canDeclareConcealedKang(_hand);
    }
}
std::map<MeldType, std::vector<meld>> Player::getOptions() {
    return options;
}
std::pair<Declaration, int> Player::getDeclaration() {
    return curr_declaration;
}
void Player::setDeclaration(std::pair<Declaration, int> declaration) {
    curr_declaration = declaration;
}

bool Player::hasHandSize(int _size) {
    return hand.size() == _size;
}
void Player::takeTile(tile_t tile) {
    if (tile->type == BONUS)
        bonuses.push_back(std::move(tile));
    else
        hand.push_back(std::move(tile));
}
Tile *Player::discardTile(int selected_index) {
    if (0 <= selected_index && selected_index < hand.size()) {
        int index = selected_index;
        Tile *discardedTile = hand[selected_index].get();
        std::cout << "Tile being discarded: " << discardedTile->get_val() << std::endl;
        discards.push_back(std::move(hand[selected_index]));
        hand.erase(hand.begin() + index);
        sort(hand.begin(), hand.end(), SortTiles<player_tile_t>());
		return discards.back().get();
    } else {
        return nullptr;
    }
}
void Player::makeMeld(player_tile_t discardedTile) {
    Declaration declaration = curr_declaration.first;
    int indexIntoOptions = curr_declaration.second;

    meld meld_to_make = options[declarationToMeld[declaration]][indexIntoOptions];

    switch (declaration) {
		case Declaration::MAHJONG:
			// self draw, last turn
			if (discardedTile) { 
				/** create copy of tiles ithout adding tile yet, check for mahjong, 
					set winning tile to tile not added yet, set score */
				// set score
			}
			hand.push_back(std::move(discardedTile));
			break;
        case Declaration::SMALL_MELDED_KANG: {
            int pengIndex = meld_to_make.indexInMeld;
            melds[pengIndex].first = SMALL_MELDED_KANG;
			// hand[meld_to_make.indexInHand] is the matching tile
			melds[pengIndex].second.push_back(hand[meld_to_make.indexInHand]);
            // tile that's moved to melds should no longer be in hand
            hand.erase(hand.begin() + meld_to_make.indexInHand);
            break;
        }
        case Declaration::CONCEALED_KANG:
        case Declaration::MELDED_KANG: // Big Melded Kang
        case Declaration::MELDED_PENG:
        case Declaration::MELDED_CHI: {
            std::pair<MeldType, player_tiles_t> new_meld;
            new_meld.first = declarationToMeld[declaration];
            assert(discardedTile);
            std::vector<int> indicesInHand = meld_to_make.indicesInHand;
            int lastDeleted = 0;
            for (int i : indicesInHand) {
                // signifies to push the discarded tile into tilesInNewMeld and that CHI was declared
                if (i == -1) {
                    new_meld.second.push_back(std::move(discardedTile));
                    //hand.erase(hand.begin() + (i - lastDeleted));
                } else {
					new_meld.second.push_back(hand[i]);
                    hand.erase(hand.begin() + (i - lastDeleted)); /** since making changes to the hand while
                    ** trying to access indices that refer to the unamended hand */
                    lastDeleted++;
                }
            }
            if (declaration != Declaration::MELDED_CHI)
                new_meld.second.push_back(std::move(discardedTile));
            melds.push_back(new_meld);
            break;
        }
    }
    setDeclaration(std::make_pair(Declaration::NONE, -1));
}
void Player::sortHand() {
    std::sort(hand.begin(), hand.end(), SortTiles<player_tile_t>());
}
