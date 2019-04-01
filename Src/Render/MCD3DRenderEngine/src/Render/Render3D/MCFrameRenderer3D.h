#pragma once

#include "../Common/MCFrameRendererBase.h"

using Microsoft::WRL::ComPtr;

namespace MC {

	class MCFrameRenderer3D : public MCFrameRendererBase 
	{
	public: /* ctor / dtor / assignment */
		MCFrameRenderer3D(const std::string& name);
		~MCFrameRenderer3D();
		MCFrameRenderer3D(MCFrameRenderer3D&)              = delete;
		MCFrameRenderer3D(MCFrameRenderer3D&&)             = delete;
		MCFrameRenderer3D& operator= (MCFrameRenderer3D&)  = delete;
		MCFrameRenderer3D& operator= (MCFrameRenderer3D&&) = delete;

	public: /* Render */
		void Render(const MCFrame3D& frame);

	private: /* Initialize methods */
		void InitializeRenderer3D();

		/* Initialize the descriptor heaps. */
		void InitializeDescriptorHeaps();

	private:
		ComPtr<ID3D12DescriptorHeap> _pCBVDescriptorHeap;
		UINT                         _CBVDescriptorIncrementSize;
	};

}

