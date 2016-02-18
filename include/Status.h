#ifndef STATUS_H
#define STATUS_H
#include <string>
#include "Player.h"
#define IN_PLAY_STATUSES \
    X(WAITING_FOR_INPUT_AFTER_DRAW) \
    X(WAITING_FOR_INPUT_ON_DISCARD) \
    X(WAITING_FOR_INPUT_TO_DISCARD) \
    X(WAITING_FOR_INPUT_AFTER_DECLARATION)
#define MAHJONG_WIN_TYPES \
	X(SELF_DRAW) \
	X(LAST_TURN) \
	X(TILE_REPLACEMENT) \
	X(ROBBING_A_KANG)

class Status {
  public:
    virtual ~Status() {};
    friend bool const operator==(const Status &a, const Status &b);
    friend bool const operator!=(const Status &a, const Status &b);
    std::string toString() const { return repr; };
  protected:
    std::string repr;
};

class Outcome {
public:
	virtual ~Outcome() {};
	virtual  std::string toString() { return repr; };
protected:
	std::string repr;
};
#define X(a) a,
enum MahjongType {
	MAHJONG_WIN_TYPES
	REGULAR_DISCARD
};
#undef X
#define X(a) #a,
static std::string win_type_strings[] = { MAHJONG_WIN_TYPES };
#undef X
class Winner : public Outcome {
public:
	wind playerWind;
	MahjongType type_of_win;
	Winner(wind _playerWind, MahjongType _type_of_win);
};
class GameDraw : public Outcome {
public:
	GameDraw() { repr = "Draw"; };
};

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
#endif // STATUS_H
