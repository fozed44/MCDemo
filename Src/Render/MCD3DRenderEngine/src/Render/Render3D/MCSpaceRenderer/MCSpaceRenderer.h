#pragma once

#include "../../../../../../Common/MCCommon/src/Data/MCResult.h"
#include "../../../../../../Common/MCCommon/src/Data/MCFrame.h"
#include "../../Common/MCFrameRenderTargetInfo.h"
#include "../MCFrameRenderer3D.h"

namespace MC {

	class MCSpaceRenderer : MCFrameRenderer3D {
	public: /* ctor */
		MCSpaceRenderer(const std::string& name, unsigned int frameIndex);
		~MCSpaceRenderer();
		MCSpaceRenderer(MCSpaceRenderer&)              = delete;
		MCSpaceRenderer(MCSpaceRenderer&&)             = delete;
		MCSpaceRenderer& operator= (MCSpaceRenderer&)  = delete;
		MCSpaceRenderer& operator= (MCSpaceRenderer&&) = delete;

	public: /* Render Frame */
		void RenderFrame(std::unique_ptr<MCSpaceFrame> pFrame, const MCFrameRenderTargetInfo& targetInfo);

	public: /* Render options. */

		// SetRenderOptions is not thread safe and must be synchronized.
		void SetRenderOptions(const MCFrameRenderOptions& options);
		MCFrameRenderOptions GetRenderOptions() const;

	private: /* Initialization */
		void InitializeSpaceRenderer();

		/* Initialize the descriptor heaps. */
		void InitializeDescriptorHeaps();

	private:
		ComPtr<ID3D12DescriptorHeap> _pCBVDescriptorHeap;
		UINT                         _CBVDescriptorIncrementSize;
		MCFrameRenderOptions         _renderOptions;
	};

}