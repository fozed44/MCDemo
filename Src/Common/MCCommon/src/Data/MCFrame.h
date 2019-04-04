#pragma once

#include "MCPrimitives.h"
#include <vector>

namespace MC {

	struct MCFrameRenderOptions {
		bool EnableUI;
		bool WireFrame;
	};

	typedef void* MCFrameItemHandle;

	struct MCCamera {
		MCFLOAT3 Position;
		MCFLOAT3 LookAt;
	};

	struct MCFrameRenderItem2D {
		MCFrameItemHandle hRenderItem;
		MCFLOAT2          Position;
		MCFLOAT2          Look;
	};

	struct MCFrameRenderItem3D {
		MCFrameItemHandle hRenderItem;
		MCFLOAT3          Position;
		MCFLOAT3          Look;
	};

	typedef enum MC_FRAME_TYPE {
		MC_FRAME_TYPE_INVALID       = 0,
		MC_FRAME_TYPE_MCFRAME_SPACE = 1
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
	struct MCFrame {
		/* The frame type. */
		MC_FRAME_TYPE FrameType;
		/* The current frame time. */
		float Time;
		/* The camera position. */
		MCCamera Camera;
	};

	struct MCFrame3D : public MCFrame {
		std::vector<MCFrameRenderItem3D> RenderItems;
	};

	struct MCSpaceFrame : public MCFrame3D {};

	struct MCFrame2D : public MCFrame { };

}
