#pragma once

#include "dxgi1_6.h"
#include "d3d12.h"
#include "d3dx12.h"
#include "DirectxMath.h"
#include "windows.h"
#include "wrl.h"
#include <memory>

// Used for error detection.
#define MAX_VALID_WINDOW_WIDTH  10000
#define MAX_VALID_WINDOW_HEIGHT 10000

// Number of frames used in the swap chain
#define FRAME_BUFFER_COUNT 2
