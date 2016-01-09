#ifndef AIPLAYER_H
#define AIPLAYER_H
#include "Player.h"
#define DECLARATIONS \
    X(SMALL_MELDED_KANG) \
    X(CONCEALED_KANG) \
    X(MELDED_PENG) \
    X(MELDED_CHI) \
    X(MELDED_KANG) \
    X(MAHJONG)
enum class Declaration;
class AIPlayer : public Player {
    public:
        AIPlayer();
        Declaration getDeclaration();
        Tile *discardTile(int selected_index=0);
        ~AIPlayer();
    private:
};

#define X(a) a,
enum class Declaration {
    NONE,
    DECLARATIONS
};
#undef X


#endif // AIPLAYER_H
