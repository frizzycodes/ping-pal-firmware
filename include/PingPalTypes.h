#pragma once
#include <cstdint>
enum class State : uint8_t
{
    BOOT,
    SETUP_MODE,
    CONNECTING_WIFI,
    ONLINE_PING_OK,
    ONLINE_PING_FAIL,
    WIFI_DISCONNECTED,
    ERROR_STATE
};

struct PingResult
{
    bool success;
    uint32_t latency_ms;
};