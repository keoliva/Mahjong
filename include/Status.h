#ifndef STATUS_H
#define STATUS_H
#include <string>
#include "Player.h"

class Status {
  public:
    virtual ~Status() {};
    friend bool const operator==(const Status &a, const Status &b);
};

class Outcome;
class Over : public Status {
  public:
    Outcome *outcome;
    Over(Outcome *_outcome) : outcome(_outcome) {};
    ~Over() { delete outcome; };
};
enum In_Play_Status {
    WAITING_FOR_INPUT_AFTER_DRAW,
    WAITING_FOR_INPUT_ON_DISCARD
};
class In_Play : public Status {
  public:
    In_Play_Status status;
    wind playerWind;
    MeldType playerMeld;
    In_Play() {};
    In_Play(In_Play_Status _status) : status(_status) {};
    In_Play(wind _wind, MeldType dec) : playerWind(_wind), playerMeld(dec) {};
};

class Outcome {
  public:
    virtual ~Outcome() {};
};
class Winner : public Outcome {
  public:
    Player player;
    Winner (Player _player): player(_player) {};
};
class GameDraw : public Outcome {};

#endif // STATUS_H
