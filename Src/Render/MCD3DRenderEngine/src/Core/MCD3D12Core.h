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
const int FRAME_BUFFER_COUNT = 2; // MUST be 2. 1 Front buffer and 1 Back buffer.

#define BACK_BUFFER_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM
