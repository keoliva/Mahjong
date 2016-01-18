#include "include/Turn.h"
#include "include/AIPlayer.h"
#include "include/HumanPlayer.h"
#include <GL/glut.h>
#include <iostream>
#include <utility>

void Turn::update()
{
    try {
        switch (state_machine.getCurrentState()) {
            case State::DISCARD_TILE:
                discardTile();
                break;
            case State::DETERMINE_ORDER_OF_CLAIMS:
                determineOrderOfClaims();
                break;
            case State::DRAW_TILE:
                drawTile();
                break;
            case State::MAKE_DECLARATION:
                makeDeclaration();
                break;
        }
    } catch (int e) {
        state_machine.pushState(State::DRAW_TILE);
        drawTile();
    }
}

void Turn::drawTile() {
    HumanPlayer *human = dynamic_cast<HumanPlayer*>(curr_player);
    //Declaration declarations[3] = {Declaration::CONCEALED_KANG, Declaration::SMALL_MELDED_KANG, Declaration::NONE};
    if (human && human->hasProvidedInput()) {
        human->providedInput = false; // toggle the value
        std::pair<Declaration, int> declaration = human->getDeclaration();
        state_machine.popState();
        game_instance->updateStatus(In_Play(WAITING_FOR_INPUT_AFTER_DECLARATION));
        if (declaration.first == Declaration::SMALL_MELDED_KANG) {
            state_machine.pushState(State::SMALL_MELDED_KANG);
        } else if (declaration.first == Declaration::CONCEALED_KANG) {
            state_machine.pushState(State::CONCEALED_KANG);
        }  else if (declaration.first == Declaration::NONE) {
            state_machine.pushState(State::DISCARD_TILE);
        }
        return;
    }
    std::cout << "Player " << wind_strings[curr_player->_wind] << "is drawing a tile from the wall" << std::endl;
    try {
        game_instance->takeFromWall(curr_player);
        std::cout << "COMPLETE!" << std::endl;
    } catch (NoMoreTilesError &e) {
        std::cout << "woops. no more tiles to grab..." << std::endl;
        game_instance->updateStatus(Over(new GameDraw()));
        state_machine.popState();
        return;
    }
    curr_player->determineOptions();
    if (!human) {
        std::pair<Declaration, int> declaration = curr_player->getDeclaration();
        if (declaration.first == Declaration::SMALL_MELDED_KANG) {
            state_machine.popState();
            state_machine.pushState(State::SMALL_MELDED_KANG);
            return;
        } else if (declaration.first == Declaration::CONCEALED_KANG) {
            state_machine.popState();
            state_machine.pushState(State::CONCEALED_KANG);
            return;
        }
    } else {
        human->setStatus(PlayerStatus::DREW_TILE);
        std::map<MeldType, std::vector<meld>> options = human->getOptions();
        // see if human player CAN declare either one of these melds
        if (!options[CONCEALED_KANG].empty() || !options[SMALL_MELDED_KANG].empty()) {
            game_instance->updateStatus(In_Play(WAITING_FOR_INPUT_AFTER_DRAW));
            return;
        }
    }
    game_instance->updateStatus(In_Play());
    state_machine.popState();
    state_machine.pushState(State::DISCARD_TILE);
}

void Turn::discardTile() {
    AIPlayer *ai = dynamic_cast<AIPlayer*>(curr_player);
    HumanPlayer *human = dynamic_cast<HumanPlayer*>(curr_player);
    if (ai) {
        ai->determineTileToDiscard();
        game_instance->setDiscard(ai->discardTile());
    } else {
        std::cout << "HUMAN DISCARDING TILE" << std::endl;
        if (human->hasProvidedInput()) {
            std::cout << "APPARENTLY I'VE PROVIDED INPUT" << std::endl;
            human->providedInput = false; // toggle the value
            game_instance->setDiscard(curr_player->discards.back());
        } else {
            game_instance->updateStatus(In_Play(WAITING_FOR_INPUT_TO_DISCARD));
            return;
        }
    }
    game_instance->updateStatus(In_Play());
    state_machine.popState();
    state_machine.pushState(State::DETERMINE_ORDER_OF_CLAIMS);
}

void Turn::determineOrderOfClaims() {
    AIPlayer *ai = dynamic_cast<AIPlayer*>(curr_player);
    HumanPlayer *human = dynamic_cast<HumanPlayer*>(game_instance->getPlayer(game_instance->humanPlayerIndex));
    Declaration max_declaration = Declaration::NONE; // declaration with the lowest value
    Player *player_highest_declaration = nullptr, *player_with_claim = nullptr;
    if (ai) {
        if (human->hasProvidedInput()) {
            human->providedInput = false; // toggle the value
            std::pair<Declaration, int> declaration = human->getDeclaration();
            if (declaration.first != Declaration::NONE) { /** meaning declaration.first is in
                    ** [Declaration::MELDED_PENG, Declaration::MELDED_KANG, Declaration::MELDED_CHI] */
                max_declaration = declaration.first;
                player_highest_declaration = game_instance->getPlayer(game_instance->humanPlayerIndex);
            }
            // forget human and curr_player, see who has precdence
            Player *players[2], *player;
            int humanPlayerIndex = game_instance->humanPlayerIndex;
            int i = (humanPlayerIndex + 1) % NUM_PLAYERS, j = 0;
            for (; i != humanPlayerIndex; i=(i+1)%NUM_PLAYERS) {
                player = game_instance->getPlayer(i);
                if (*player != *curr_player) {
                    std::cout << "determining if " << wind_strings[player->_wind] << " can geeet ittt??????" << std::endl;
                    player->determineOptions(game_instance->getDiscard());
                    players[j++] = player;
                }
            }
            player_with_claim =
                determineWhoHasPrecedence(max_declaration, player_highest_declaration, players, 2);
            std::cout << "determined player with claim after human gave their input.........." << std::endl;
        } else {
            std::cout << "determining of human can geeeeeet iiiitt??????????????"<<std::endl;
            human->determineOptions(game_instance->getDiscard());
            std::cout << "finished getting options??????????????"<<std::endl;
            std::map<MeldType, std::vector<meld>> options = human->getOptions();
            std::cout << "assigned options to local variable............"<<std::endl;
            // see if human player CAN declare either one of these melds
            if (!options[PENG].empty() || !options[KANG].empty() || !options[CHI].empty()) {
                std::cout << "before status update.........."<<std::endl;
                game_instance->updateStatus(In_Play(WAITING_FOR_INPUT_ON_DISCARD));
                std::cout << "after status update********************NEED TO WAIT ON HUMAN TO DECIDE IF SHE WANTS DISCARD***********" << std::endl;
                std::cout << "has provided bool: " << human->hasProvidedInput() << std::endl;
                std::cout << std::endl;
                return;
            }
            std::cout << "no you can't" << std::endl;
        }
    } else {
        // forget human and see who has precdence
        Player *players[3], *player, *human_player = game_instance->getPlayer(game_instance->humanPlayerIndex);
        int humanPlayerIndex = game_instance->humanPlayerIndex;
        int i = (humanPlayerIndex + 1) % NUM_PLAYERS, j = 0;
        for (; i != humanPlayerIndex; i=(i+1)%NUM_PLAYERS) {
            player = game_instance->getPlayer(i);
            if (*player != *human_player) {
                player->determineOptions(game_instance->getDiscard());
                players[j++] = player;
            }
        }
        player_with_claim =
            determineWhoHasPrecedence(max_declaration, player_highest_declaration, players, 3);
        std::cout << "player didn't want or couldn't take discrd.........." << std::endl;
    }
    game_instance->updateStatus(In_Play());
    if (player_with_claim) {
        std::cout << "Player " << " " << wind_strings[player_with_claim->_wind] << " got the claim!!!!!!!!!!!!!" << std::endl;
        int num_times_to_cycle = player_with_claim->_wind - curr_player->_wind;
        if (num_times_to_cycle < 0)
            num_times_to_cycle += NUM_PLAYERS;
        for (int i = 0; i < num_times_to_cycle; i++)
            game_instance->cycleCurrentPlayer();
        std::cout << "current player is now human? " << (dynamic_cast<HumanPlayer*>(curr_player)) << std::endl;
        state_machine.popState();
        state_machine.pushState(State::MAKE_DECLARATION);
    } else {
        game_instance->setDiscard(nullptr);
        state_machine.popState();
        game_instance->cycleCurrentPlayer();
        state_machine.pushState(State::DRAW_TILE);
    }
}

Player *Turn::determineWhoHasPrecedence(Declaration max_declaration, Player *player_highest_declaration,
                                       Player *players[], int num_players) {
    Player *player;
    for (int i = 0; i < num_players; i++) {
        player = players[i];
        if (player->getDeclaration().first > max_declaration) {
            max_declaration = player->getDeclaration().first;
            player_highest_declaration = player;
        }
    }
    return player_highest_declaration;
}

void Turn::makeDeclaration() {
    std::cout << "in make DECLARATIONNNNNNNNNN. cur__player: " << curr_player << std::endl;
    HumanPlayer *human = dynamic_cast<HumanPlayer*>(curr_player);
    if (human) {
        Declaration dec_type = curr_player->getDeclaration().first;
        MeldType meld_type = Player::declarationToMeld[dec_type];
        std::vector<meld> melds = curr_player->getOptions()[meld_type]; // key is MeldType
        std::cout << "HUMAN HERE MAKING DECLARATIONNNNNNNNNNNNNNNNNNNNN" << std::endl;
        if (melds.size() == 1) {
            curr_player->setDeclaration(std::make_pair(dec_type, 0));
            std::cout << "HUMAN HERE MAKING DECLARATION. HAD ONLY ONE OPTION SO GONNA GO AHEADDDDDDDDD" << std::endl;
        } else { // must choose from several options
            // means human hasn't chosen which sets of tiles to kang
            if (human->hasProvidedInput()) {
                human->providedInput = false;
            } else { // meaning curr_player->getDeclaration().second == -1
                game_instance->updateStatus(In_Play(WAITING_FOR_INPUT_AFTER_DECLARATION));
                return;
            }
        }
    }
    game_instance->updateStatus(In_Play(curr_player->_wind,
                                Player::declarationToMeld[curr_player->getDeclaration().first]));
    std::cout << "AKING MELDDDDDDDDDDDDDD" << std::endl;
    Tile *currD = game_instance->getDiscard();
    std::cout << "DISCARDEDTILE == " << currD << std::endl;
    curr_player->makeMeld(currD);
    game_instance->setDiscard(nullptr);
    state_machine.popState();
    state_machine.pushState(State::DISCARD_TILE);
}

Turn::~Turn() {
    //delete state_machine;
}
