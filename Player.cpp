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
    if (discardedTile) {
        options[PENG] = handEvaluator->canDeclareMeldedPeng(hand, discardedTile);
        cout << "=====================" << endl;
        cout << "PENG SIZE: " << options[PENG].size() << endl;
        cout << "PENG EMPTY? " << options[PENG].empty() << endl;
        for (meld _meld : options[PENG]) {
            cout << "meld..." << endl;
            for (int i : _meld.indicesInHand) {
                cout << i << ", ";
            }
            cout << endl;
        }
        cout << "=================" << endl;
        cout << endl;
        options[KANG] = handEvaluator->canDeclareBigMeldedKang(hand, discardedTile);
        cout << "=====================" << endl;
        cout << "KANG SIZE: " << options[KANG].size() << endl;
        cout << "KANG EMPTY? " << options[KANG].empty() << endl;
        for (meld _meld : options[KANG]) {
            cout << "meld..." << endl;
            for (int i : _meld.indicesInHand) {
                cout << i << ", ";
            }
            cout << endl;
        }
        cout << "=================" << endl;
        cout << endl;
        options[CHI] = handEvaluator->canDeclareMeldedChi(hand, discardedTile);
        cout << "=====================" << endl;
        cout << "CHI SIZE: " << options[CHI].size() << endl;
        cout << "CHI EMPTY? " << options[CHI].empty() << endl;
        for (meld _meld : options[CHI]) {
            cout << "meld..." << endl;
            for (int i : _meld.indicesInHand) {
                cout << i << ", ";
            }
            cout << endl;
        }
        cout << "=================" << endl;
        cout << endl;
    } else {
        options[SMALL_MELDED_KANG] = handEvaluator->canDeclareSmallMeldedKang(melds, hand);
        cout << "=====================" << endl;
        cout << "SMALL_MELDED_KANG SIZE: " << options[SMALL_MELDED_KANG].size() << endl;
        cout << "SMALL_MELDED_KANG EMPTY? " << options[SMALL_MELDED_KANG].empty() << endl;
        for (meld _meld : options[SMALL_MELDED_KANG]) {
            cout << "meld..." << endl;
            cout << "indexInHand: " << _meld.indexInHand << endl;
            cout << "indexInMeld: " << _meld.indexInMeld << endl;
            cout << endl;
        }
        cout << "=================" << endl;
        cout << endl;
        options[CONCEALED_KANG] = handEvaluator->canDeclareConcealedKang(hand);
        cout << "=====================" << endl;
        cout << "CONCEALED_KANG SIZE: " << options[CONCEALED_KANG].size() << endl;
        cout << "CONCEALED_KANG EMPTY? " << options[CONCEALED_KANG].empty() << endl;
        for (meld _meld : options[CONCEALED_KANG]) {
            cout << "meld..." << endl;
            for (int i : _meld.indicesInHand) {
                cout << i << ", ";
            }
            cout << endl;
        }
        cout << "=================" << endl;
        cout << endl;
    }
}
std::map<MeldType, std::vector<meld>> Player::getOptions(Tile *discardedTile) {
    return options;
}
std::pair<Declaration, int> Player::getDeclaration() {
    return curr_declaration;
}
void Player::setDeclaration(std::pair<Declaration, int> declaration) {
    curr_declaration = declaration;
}
bool Player::declarationIn(Declaration declarations[]) {
    int _size = sizeof(declarations)/sizeof(declarations[0]);
    Declaration declaration = curr_declaration.first;
    for (int i = 0; i < _size; i++) {
        if (declaration == declarations[i]) return true;
    }
    return false;
}
bool Player::hasHandSize(int _size) {
    return hand.size() == _size;
}
void Player::takeTile(Tile *tile) {
    if (tile->type == BONUS)
        bonuses.push_back(tile);
    else
        hand.push_back(tile);
}
Tile *Player::discardTile(int selected_index) {
    if (0 <= selected_index && selected_index < hand.size()) {
        int index = selected_index;
        Tile *discardedTile = hand[selected_index];
        std::cout << "Tile being discarded: " << discardedTile->get_val() << std::endl;
        discards.push_back(discardedTile);
        hand.erase(hand.begin() + index);
        sort(hand.begin(), hand.end(), SortTiles());
        return discardedTile;
    } else {
        return nullptr;
    }
}
void Player::makeMeld(Tile *discardedTile) {
    Declaration declaration = curr_declaration.first;
    int indexIntoOptions = curr_declaration.second;

    meld meld_to_make = options[declarationToMeld[declaration]][indexIntoOptions];

    switch (declaration) {
        case Declaration::SMALL_MELDED_KANG: {
            int pengIndex = meld_to_make.indexInMeld;
            Tile *matching_tile = hand[meld_to_make.indexInHand];
            melds[pengIndex].first = SMALL_MELDED_KANG;
            melds[pengIndex].second.push_back(matching_tile);
            // tile that's moved to melds should no longer be in hand
            hand.erase(hand.begin() + meld_to_make.indexInHand);
            break;
        }
        case Declaration::CONCEALED_KANG:
        case Declaration::MELDED_KANG: // Big Melded Kang
        case Declaration::MELDED_PENG:
        case Declaration::MELDED_CHI: {
            std::pair<MeldType, std::vector<Tile*>> new_meld;
            new_meld.first = declarationToMeld[declaration];
            assert(discardedTile);
            std::vector<int> indicesInHand = meld_to_make.indicesInHand;
            std::vector<Tile*> tilesInNewMeld, handCopy = hand;
            int lastDeleted = 0;
            for (int i : indicesInHand) {
                // signifies to push the discarded tile into tilesInNewMeld and that CHI was declared
                if (i == -1) {
                    tilesInNewMeld.push_back(discardedTile);
                    hand.erase(hand.begin() + (i - lastDeleted));
                } else {
                    tilesInNewMeld.push_back(handCopy[i]);
                    hand.erase(hand.begin() + (i - lastDeleted)); /** since making changes to the hand while
                    ** trying to access indices that refer to the unamended hand */
                    lastDeleted++;
                }
            }
            if (declaration != Declaration::MELDED_CHI)
                tilesInNewMeld.push_back(discardedTile);
            new_meld.second = tilesInNewMeld;
            melds.push_back(new_meld);
            break;
        }
    }
    setDeclaration(std::make_pair(Declaration::NONE, -1));
}
void Player::sortHand() {
    std::sort(hand.begin(), hand.end(), SortTiles());
}
