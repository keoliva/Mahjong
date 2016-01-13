#ifndef STATUS_H
#define STATUS_H
#include <string>
#include "Player.h"
#define IN_PLAY_STATUSES \
    X(WAITING_FOR_INPUT_AFTER_DRAW) \
    X(WAITING_FOR_INPUT_ON_DISCARD) \
    X(WAITING_FOR_INPUT_TO_DISCARD) \
    X(WAITING_FOR_INPUT_AFTER_DECLARATION)

class Status {
  public:
    virtual ~Status() {};
    friend bool const operator==(const Status &a, const Status &b);
    friend bool const operator!=(const Status &a, const Status &b);
    std::string toString() const { return repr; };
  protected:
    std::string repr;
};

class Outcome;
class Over : public Status {
  public:
    Outcome *outcome;
    Over(Outcome *_outcome);
    ~Over() { delete outcome; };
};
#define X(a) a,
enum In_Play_Status {
    IN_PLAY_STATUSES
};
#undef X
#define X(a) #a,
static std::string in_play_status_strings[] = { IN_PLAY_STATUSES };
#undef X
class In_Play : public Status {
  public:
    In_Play_Status status;
    wind playerWind;
    MeldType playerMeld;
    In_Play();
    In_Play(In_Play_Status _status);
    In_Play(wind _wind, MeldType dec); // MeldType defined in HandEvaluator.h
};

class Outcome {
  public:
    virtual ~Outcome() {};
    virtual  std::string toString() { return repr; };
  protected:
    std::string repr;
};
class Winner : public Outcome {
  public:
    Player *player;
    Winner (Player *_player);
};
class GameDraw : public Outcome {
  public:
    GameDraw() { repr = "Draw"; };
};

#endif // STATUS_H
