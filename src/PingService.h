#pragma once
#include <stdint.h>

enum class PingStatus : uint8_t
{
    NONE,
    SUCCESS,
    FAIL
};

class PingService
{
private:
    const char *targetHost;
    unsigned long intervalMs;
    unsigned long lastPingTime;
    bool enabled;

public:
    PingService();

    void enable();
    void disable();

    void setTarget(const char *host);
    void setInterval(unsigned long ms);

    PingStatus update();
};
