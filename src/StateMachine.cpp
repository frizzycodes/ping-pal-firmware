#include "StateMachine.h"
#include <cstdio>

StateMachine::StateMachine()
{
    currentState = State::BOOT;
}
State StateMachine::getCurrentState() const
{
    return currentState;
}
void StateMachine::transitionTo(State newState)
{
    if (currentState == newState)
    {
        return;
    }
    // Log state change replace it with LED later
    printf("STATE CHANGE: %d -> %d\n", static_cast<int>(currentState), static_cast<int>(newState));
    currentState = newState;
}
void StateMachine::update()
{
    switch (currentState)
    {
    case State::BOOT:
        // Eventually: initialize stuff and logic
        break;
    case State::SETUP_MODE:
        break;
    case State::CONNECTING_WIFI:
        break;
    case State::ONLINE_PING_OK:
        break;
    case State::ONLINE_PING_FAIL:
        break;
    case State::WIFI_DISCONNECTED:
        break;
    case State::ERROR_STATE:
        break;
    }
}