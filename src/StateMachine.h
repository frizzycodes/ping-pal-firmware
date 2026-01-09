#pragma once
#include "PingPalTypes.h"

class StateMachine
{
private:
    State currentState;

public:
    StateMachine();
    void update();
    void transitionTo(State newState);
    State getCurrentState() const;
};
