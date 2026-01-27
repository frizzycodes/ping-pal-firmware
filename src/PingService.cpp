#include "PingService.h"
#include <WiFi.h>
#include <ESP32Ping.h>
#include <WiFiGeneric.h>

PingService::PingService()
    : targetHost("8.8.8.8"),
      intervalMs(10000),
      lastPingTime(0),
      lastPingLatency(0),
      lastStatus(PingStatus::NONE),
      enabled(false),
      pingTaskHandle(nullptr)
{
}

void PingService::enable()
{
    if (enabled)
        return;

    enabled = true;
    lastStatus = PingStatus::NONE;

    // ===== CREATE PING TASK ON CORE 0 =====
    xTaskCreatePinnedToCore(
        pingTaskThunk,
        "PingTask",
        4096,
        this,
        1,
        &pingTaskHandle,
        0 // Core 0
    );
}

void PingService::disable()
{
    enabled = false;

    if (pingTaskHandle)
    {
        vTaskDelete(pingTaskHandle);
        pingTaskHandle = nullptr;
    }
}
void PingService::pingTaskThunk(void *param)
{
    static_cast<PingService *>(param)->pingTaskLoop();
}

void PingService::setTarget(const char *host)
{
    targetHost = host;
}

PingStatus PingService::pollResult()
{
    PingStatus s = lastStatus;
    lastStatus = PingStatus::NONE;
    return s;
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

void PingService::pingTaskLoop()
{
    for (;;)
    {
        if (!enabled)
            vTaskDelay(pdMS_TO_TICKS(500));

        unsigned long now = millis();

        if (now - lastPingTime >= intervalMs)
        {
            lastPingTime = now;

            bool ok = Ping.ping(targetHost, 4);

            if (ok)
            {
                lastPingLatency = Ping.averageTime();
                lastStatus = PingStatus::SUCCESS;
            }
            else
            {
                lastPingLatency = 0;
                lastStatus = PingStatus::FAIL;
            }
        }

        // Yield to system (VERY IMPORTANT)
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}