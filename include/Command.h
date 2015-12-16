#ifndef COMMAND_H
#define COMMAND_H
#include "Player.h"

class Command
{
    public:
        virtual void execute(Player *player) = 0;
        virtual ~Command() {};
    protected:
    private:
};

class DiscardTile : public Command {
    public:
        DiscardTile(int index) : selectedTileIndex(index) {};
        virtual void execute(Player *player) {
            player->discardTile(selectedTileIndex);
        };
    private:
        int selectedTileIndex;
};

class NullCommand : public Command {
    public:
        NullCommand() {};
        virtual void execute(Player *player) { Command::execute(player); };
};

#endif // COMMAND_H
