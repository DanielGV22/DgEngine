#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// windows network libraries
#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

// shared libraries
#include <Core/Inc/Core.h>
#include <Math/Inc/DWMath.h>
#include <Graphics/Inc/Graphics.h>
#include <Physics/Inc/Physics.h>

#define LATENCY_CHECK 5
#define LATENCY_DEV 150
#define WM_SOCKET (WM_USER + 1)
#define DEFAULT_PORT 8000
#define RECEIVE_BUFFER_SIZE 65535 // max 16 value
#define POSITION_UPDATE_RATE 1.0f/30.0f; // 30 fps