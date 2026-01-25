#include "PingService.h"
#include <WiFi.h>
#include <ESP32Ping.h>
#include <WiFiGeneric.h>

PingService::PingService()
    : targetHost("8.8.8.8"),
      intervalMs(10000),
      lastPingTime(0),
      lastPingLatency(0),
      enabled(false)
{
}

void PingService::enable()
{
    enabled = true;
}

void PingService::disable()
{
    enabled = false;
}

void PingService::setTarget(const char *host)
{
    targetHost = host;
}

bool PingService::getStatus()
{
    return enabled;
}

unsigned long PingService::getLastPingLatency()
{
    return lastPingLatency;
}

unsigned long PingService::getLastPingTime()
{
    return lastPingTime;
}
unsigned long PingService::getInterval()
{
    return intervalMs;
}

const char *PingService::getTarget() const
{
    return targetHost;
}

void PingService::setInterval(unsigned long ms)
{
    intervalMs = ms;
}

PingStatus PingService::update()
{
    if (!enabled)
        return PingStatus::NONE;

    unsigned long now = millis();

    // Not time yet → do nothing
    if (now - lastPingTime < intervalMs)
        return PingStatus::NONE;

    // Mark ping attempt time *before* blocking call
    lastPingTime = now;

    // Blocking ICMP ping (library limitation)
    bool success = Ping.ping(targetHost, 4);

    if (success)
    {
        lastPingLatency = Ping.averageTime();
        return PingStatus::SUCCESS;
    }
    else
    {
        lastPingLatency = 0;
        return PingStatus::FAIL;
    }
}