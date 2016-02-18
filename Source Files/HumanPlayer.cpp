#include "include/HumanPlayer.h"

HumanPlayer::HumanPlayer() {
    providedInput = false;
    curr_status = PlayerStatus::NONE;
}
bool HumanPlayer::hasProvidedInput() {
    return providedInput;
}
void HumanPlayer::takeTile(tile_t tile) {
    providedInput = false;
    Player::takeTile(std::move(tile));
}

Tile * HumanPlayer::discardTile(int selected_index) {
    providedInput = true;
    curr_status = PlayerStatus::DISCARDED_TILE;
    return Player::discardTile(selected_index);
}

void HumanPlayer::setDeclaration(std::pair<Declaration, int> declaration) {
    providedInput = true; // whenever function is called, it's based on the human's input
    Player::setDeclaration(declaration);
}

void HumanPlayer::makeMeld(player_tile_t discardedTile) {
    Player::makeMeld(discardedTile);
    providedInput = false;
    curr_status = PlayerStatus::DECLARING_MELD;
}

HumanPlayer::~HumanPlayer() {
    //dtor
}
