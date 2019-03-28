#pragma once

namespace MC {


	struct MCVector4 {
		float x;
		float y;
		float z;
		float w;
	};

	/*
		MCFrame

		MCFrame contains all of the information required by the renderer to draw a single frame.
	*/
	struct MCFrame {

		// The position of the camera.
		MCVector4 CameraPosition;

		// The direction the camera is pointing.
		MCVector4 LookAt;

		// The current frame time;
		float Time;
	};

}
