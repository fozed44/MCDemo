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

	typedef enum MC_FRAME_TYPE {
		MC_FRAME_TYPE_INVALID = 0,
		MC_FRAME_TYPE_MCFRAME_3D,
		MC_FRAME_TYPE_MCFRAME_2D
	} MC_FRAME_TYPE;

	/*
		MCFrame

		MCFrame contains all of the information required by the renderer to draw a single frame.

		Frames are passed to MCRenderer (the main gateway to the render system) via a void*.
		This is because MCRenderer.QueueFrame is the one any only method presented to the outside
		that allows frames to be queued. The renderer is responsible for determining what will
		handle the frame based on the frame type and the state of the renderer. 

		The renderer determines the frame type based on the MC_FRAME_TYPE FrameType member
		of all frame structures. This is why all structures contain this property as the first field.
	*/
	struct MCFrame3D {
		MC_FRAME_TYPE FrameType;
		// The current frame time;
		float Time;
		// The camera position.
		MCCamera Camera;
		std::vector<MCSimpleFrameRenderItem3D> SimpleRenderItems;
	};

	struct MCFrame2D {
		MC_FRAME_TYPE FrameType;
		float Time;
		MCCamera Camera;
		std::vector<MCSimpleFrameRenderItem2D> SimpleRenderItems;
	};

}
