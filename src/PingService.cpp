#include "PingService.h"
#include <WiFi.h>
#include <ESP32Ping.h>
#include <WiFiGeneric.h>

PingService::PingService()
    : targetHost("8.8.8.8"),
      intervalMs(3000),
      lastPingTime(0),
      enabled(false)
{
}
void PingService::enable()
{
    enabled = true;
    lastPingTime = millis();
}
void PingService::disable()
{
    enabled = false;
}

void PingService::setTarget(const char *host)
{
    targetHost = host;
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
    if (millis() - lastPingTime >= intervalMs)
    {
        lastPingTime = millis();
        bool ret = Ping.ping(targetHost, 1);
        if (ret)
        {
            return PingStatus::SUCCESS;
        }
        else
        {
            return PingStatus::FAIL;
        }
    }
    return PingStatus::NONE;
}