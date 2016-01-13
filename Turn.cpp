#include "include/Turn.h"
#include "include/AIPlayer.h"
#include "include/HumanPlayer.h"
#include <GL/glut.h>
#include <iostream>
#include <utility>
int i = 0;

void Turn::update()
{
    try {
        switch (state_machine.getCurrentState()) {
            case State::DISCARD_TILE:
                discardTile();
                break;
            case State::DRAW_TILE:
                drawTile();
                break;
            case State::SMALL_MELDED_KANG:
                smallMeldedKang();
                break;
            case State::CONCEALED_KANG:
                break;
        }
    } catch (int e) {
        //game_instance->switchPlayer();
        state_machine.pushState(State::DRAW_TILE);
        drawTile();
    }
}

void Turn::drawTile() {
    HumanPlayer *human = dynamic_cast<HumanPlayer*>(curr_player);
    Declaration declarations[3] = {Declaration::CONCEALED_KANG, Declaration::SMALL_MELDED_KANG, Declaration::NONE};
    if (human && human->declarationIn(declarations)) {
        std::pair<Declaration, int> declaration = human->getDeclaration();
        state_machine.popState();
        game_instance->updateStatus(new In_Play(WAITING_FOR_INPUT_AFTER_DECLARATION));
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
    std::cout << "taking from a wall..." << i <<  std::endl;
    try {
        game_instance->takeFromWall(curr_player);
        std::cout << "COMPLETE!" << std::endl;
    } catch (NoMoreTilesError &e) {
        std::cout << "woops. no more tiles to grab..." << std::endl;
        game_instance->updateStatus(new Over(new GameDraw()));
        state_machine.popState();
        return;
    }
    curr_player->determineOptions();

    AIPlayer *ai;
    if (!human) {
        ai = dynamic_cast<AIPlayer*>(curr_player);
        std::pair<Declaration, int> declaration = ai->getDeclaration();
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
        std::map<MeldType, std::vector<meld>> options = human->getOptions();
        // see if human player CAN declare either one of these melds
        if (!options[CONCEALED_KANG].empty() || !options[SMALL_MELDED_KANG].empty()) {
            game_instance->updateStatus(new In_Play(WAITING_FOR_INPUT_AFTER_DRAW));
            return;
        }
    }
    state_machine.popState();
    state_machine.pushState(State::DISCARD_TILE);
}

void Turn::discardTile() {
    game_instance->setDiscard(curr_player->discardTile());

    std::cout << "finished discarding tile " << i++ << std::endl;
    state_machine.popState();
    game_instance->cycleCurrentPlayer();
}

void Turn::smallMeldedKang() {
    if (dynamic_cast<HumanPlayer*>(curr_player)) {
        Declaration dec_type = curr_player->getDeclaration().first;
        MeldType meld_type = Player::declarationToMeld[dec_type];
        std::vector<meld> melds = curr_player->getOptions()[meld_type]; // key is MeldType
        if (melds.size() == 1) {
            curr_player->setDeclaration(std::make_pair(dec_type, 0));
        } else { // must choose from several options
            // means human hasn't chosen which sets of tiles to kang
            if (curr_player->getDeclaration().second == -1) return;
        }
    }

    game_instance->updateStatus(new In_Play(curr_player->_wind, MeldType::KANG));
    curr_player->makeMeld();
    state_machine.popState();
    game_instance->cycleCurrentPlayer();
    state_machine.pushState(State::DRAW_TILE);
}

void Turn::concealedKang() {
    Turn::smallMeldedKang();
}

Turn::~Turn() {
    //delete state_machine;
}
