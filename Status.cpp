#include "Status.h"
#include <typeinfo>
using namespace std;

bool const operator==(const Status &a, const Status &b) {
    return a.toString() == b.toString();
}
bool const operator!=(const Status &a, const Status &b) {
    return !(a == b);
}

Over::Over(Outcome *_outcome) {
    repr = "Over<" + _outcome->toString() + ">";
}

In_Play::In_Play() {
    repr = "In_Play";
}
In_Play::In_Play(In_Play_Status _status) {
    repr = "In_Play<" + in_play_status_strings[_status] + ">";
}
In_Play::In_Play(wind _wind, MeldType dec) {
    repr = "In_Play<" + wind_strings[_wind] + ", " + meld_strings[dec] + ">";
}

Winner::Winner(Player *_player) {
    repr = "Winner<" + wind_strings[_player->_wind] + ">";
}
