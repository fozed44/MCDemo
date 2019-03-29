#include "MCFrameScheduler3D.h"

namespace MC {

	MCFrameScheduler3D::MCFrameScheduler3D()
	{
	}


	MCFrameScheduler3D::~MCFrameScheduler3D()
	{
	}

	void MCFrameScheduler3D::QueueFrame3D(MCFrame3D *pFrame) {
		_ppExecuters[0]->QueueNextFrame(pFrame);

	}

}