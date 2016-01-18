#ifndef TURN_H
#define TURN_H
#include "AIPlayer.h"
#include "Game.h"

template <typename T>
class StackFSM {
public:
    StackFSM() {};
    T getCurrentState() { if (stackOfStates.empty()) throw -1; else return stackOfStates.back(); };
    void pushState(T state) { stackOfStates.push_back(state); };
    T popState() {
        T state = stackOfStates.back();
        stackOfStates.pop_back();
        return state;
    };
private:
    std::vector<T> stackOfStates;
};
#define X(a) a,
enum class State {
    DISCARD_TILE, DETERMINE_ORDER_OF_CLAIMS,
    DRAW_TILE,
    MAKE_DECLARATION,
    DECLARATIONS // defined in AIPlayer.h
};
#undef X
class Game;
class Turn
{
    public:
        Turn() : state_machine(StackFSM<State>()) {};
        ~Turn();
        Turn(Game *game, Player *player) : game_instance(game), curr_player(player) {};
        void setCurrentPlayer(Player *player) { curr_player = player; };
        void update();
        void drawTile();
        void discardTile();
        void determineOrderOfClaims();
        Player *determineWhoHasPrecedence(Declaration max_declaration, Player *player_highest_declaration,
                                       Player *players[], int num_players);
        void makeDeclaration();
    protected:
    private:
        bool determinedWhoHasPrecedence;
        StackFSM<State> state_machine;
        Game *game_instance;
        Player *curr_player;
};

#endif // TURN_H
