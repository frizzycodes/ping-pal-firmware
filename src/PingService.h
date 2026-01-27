#pragma once
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

    volatile unsigned long lastPingTime;
    volatile unsigned long lastPingLatency;
    volatile PingStatus lastStatus;

    bool enabled;

    TaskHandle_t pingTaskHandle;
    static void pingTaskThunk(void *param);
    void pingTaskLoop();

public:
    PingService();

    void enable();
    void disable();

    PingStatus pollResult();
    unsigned long getLastPingTime();
    unsigned long getInterval();
    unsigned long getLastPingLatency();

    void setTarget(const char *host);
    const char *getTarget() const;
    void setInterval(unsigned long ms);
};
