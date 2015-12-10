#ifndef STATUS_H_INCLUDED
#define STATUS_H_INCLUDED
#include "Player.h"

class outcome {
  public:
    virtual ~outcome() {};
    virtual std::string outcome_to_string() = 0;
};
class Winner : public outcome {
  public:
    Player *player; // is a pointer because Player class might end up being an ABC
    Winner (player *p): _player(p), win(w) {};
    std::string outcome_to_string() {
        std::stringstream ss;
        ss << "Winner<" << (wind_strings[p->-wind]) << ">";
        return ss.str();
    };
};
class Draw : public outcome {
    public:
        Draw() {};
        std::sring outcome_to_string() { return "Draw"; };
};

class status {
  public:
    virtual ~status() {};
    virtual std::string status_to_string() = 0;
};
class Over : public status {
  public:
    outcome *_outcome;
    Over() {};
    Over(outcome *o) : _outcome(o) {};
    ~Over() {};
    std::string status_to_string() {
        std::stringstream ss;
        ss << "Over<" << _outcome->outcome_to_string() << ">";
        return ss.str();
    };
};
class In_Play : public status {
  public:
    ~In_Play() {};
    std::string status_to_string() { return "In_Play"; };
};

#endif // STATUS_H_INCLUDED
