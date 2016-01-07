#ifndef TURN_H
#define TURN_H
#include "Game.h"

enum class State {
    DISCARD_TILE,
    DRAW_TILE,
    SMALL_MELDED_KANG, CONCEALED_KANG,
    MELDED_PENG, MELDED_CHI, MELDED_KANG,
    MAHJONG
};

template <typename T>
class StackFSM {
public:
    StackFSM() {};
    T getCurrentState() { return (stackOfStates.empty())?NULL:stackOfStates.back(); };
    void pushState(T state) { stackOfStates.push_back(state); };
    T popState() {
        T state = stackOfStates.back();
        stackOfStates.pop_back();
        return state;
    };
private:
    std::vector<T> stackOfStates;
};

class Game;
class Turn
{
    public:
        //typedef void (Turn::*fn)();
        Turn();
        ~Turn();
        Turn(Game *game, Player *player) : game_instance(game), curr_player(player) {};
        void setCurrentPlayer(Player *player) { curr_player = player; };
        void update();
        void drawTile();
        void discardTile();
        void declareSmallMeldedKang();
        void declareConcealedKang();
    protected:
    private:
        StackFSM<State> state_machine;
        Game *game_instance;
        Player *curr_player;
};

#endif // TURN_H
