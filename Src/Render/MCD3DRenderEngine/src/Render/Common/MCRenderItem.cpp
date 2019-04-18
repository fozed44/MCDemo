#include "MCRenderItem.h"
#include "MCFrameRenderer.h"

namespace MC {

#pragma region ctor

	MCRenderItem::MCRenderItem(MCFrameRenderer* pRenderer)
		: _pRenderer{pRenderer} { }


	MCRenderItem::~MCRenderItem() { }

#pragma endregion

#pragma region Render

	MC_RESULT MCRenderItem::Render() {
		return MC_RESULT::OK;
	}

#pragma endregion
}
