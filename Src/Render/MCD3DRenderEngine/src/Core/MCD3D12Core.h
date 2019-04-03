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
const int FRAME_BUFFER_COUNT = 3;
const int DEPTH_BUFFER_COUNT = FRAME_BUFFER_COUNT - 1; // One less descriptor than there are frame buffers
									                   // (we don't need a ds for the frame that is currently displayed.)

#define BACK_BUFFER_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM
