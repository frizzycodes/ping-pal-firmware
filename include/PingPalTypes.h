#pragma once
#include <cstdint>
enum class State : uint8_t
{
    BOOT,
    SETUP_MODE,
    SETUP_CONFIRMATION,
    CONNECTING_WIFI,
    ONLINE_PINGING,
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