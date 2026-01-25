#pragma once
#include <stdint.h>

enum class PingStatus : uint8_t
{
    NONE,
    SUCCESS,
    FAIL
};
enum class PingInternalState
{
    IDLE,
    IN_PROGRESS
};

class PingService
{
private:
    const char *targetHost;
    unsigned long intervalMs;
    unsigned long lastPingTime;
    unsigned long lastPingLatency;
    bool enabled;
    PingInternalState state = PingInternalState::IDLE;
    unsigned long pingStartTime = 0;

public:
    PingService();

    void enable();
    void disable();
    bool getStatus();
    unsigned long getLastPingTime();
    unsigned long getInterval();
    unsigned long getLastPingLatency();
    void setTarget(const char *host);
    const char *getTarget() const;
    void setInterval(unsigned long ms);

    PingStatus update();
};
