#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H
#include "Command.h"

typedef struct mouseKeyActivity {
    bool mouseMoved, mouseClicked;
    int selectionIndex;
    mouseKeyActivity() : selectionIndex(0) {};
    mouseKeyActivity(bool moved, bool clicked, int index) :\
        mouseMoved(moved), mouseClicked(clicked), selectionIndex(index) {};
} mouseKeyActivity;

class InputHandler
{
    public:
        InputHandler(mouseKeyActivity &input);
        Command *handleInput();
        virtual ~InputHandler();
    private:
        mouseKeyActivity input_;
        Command *nullCommand;
        Command *discardTile;
        // if a Melded Peng, Melded Chi, or Big Melded Kang is possible for the human using the
        // most recently discarded tile, they'll be able to choose within the options
        // which they want to declare
        Command *declareMoveWithDiscard;
        Command *declareMoveWithDrawnTile;
};

#endif // INPUTHANDLER_H
