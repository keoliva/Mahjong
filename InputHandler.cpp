#include "InputHandler.h"
#include <iostream>
InputHandler::InputHandler(mouseKeyActivity &input)
{
    input_ = input;
    nullCommand = new NullCommand();
    std::cout << "inputHandler input = " << input.selectionIndex << std::endl;
    discardTile = new DiscardTile(input.selectionIndex-1);
}

Command *InputHandler::handleInput()
{
    if (input_.selectionIndex)
        return discardTile;
    else return nullCommand;
}

InputHandler::~InputHandler()
{
    delete nullCommand;
    delete discardTile;
    //delete declareMoveWithDiscard;
    //delete declareMoveWithDrawnTile;
};
