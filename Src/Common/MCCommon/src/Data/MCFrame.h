#pragma once

#include "MCPrimitives.h"
#include <vector>

namespace MC {


	typedef char* MCFrameItemHandle;

	struct MCCamera {
		MCFLOAT3 Position;
		MCFLOAT3 LookAt;
	};

	struct MCSimpleFrameRenderItem2D {
		MCFrameItemHandle Handle;
		MCFLOAT2 Position;
		MCFLOAT2 Look;
	};

	struct MCSimpleFrameRenderItem3D {
		MCFrameItemHandle Handle;
		MCFLOAT3 Position;
		MCFLOAT3 Look;
	};

	/*
		MCFrame

		MCFrame contains all of the information required by the renderer to draw a single frame.
	*/
	struct MCFrame3D {
		// The current frame time;
		float Time;
		// The camera position.
		MCCamera Camera;
		std::vector<MCSimpleFrameRenderItem3D> SimpleRenderItems;
	};

	struct MCFrame2D {
		float Time;
		MCCamera Camera;
		std::vector<MCSimpleFrameRenderItem2D> SimpleRenderItems;
	};

}
