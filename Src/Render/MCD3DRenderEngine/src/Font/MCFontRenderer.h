//#pragma once
//
//#include "MCFont.h"
//
//using Microsoft::WRL::ComPtr;
//
//namespace MC {
//
//	struct MCFontRenderOptions {
//		DirectX::XMFLOAT2 scale;
//		DirectX::XMFLOAT2 padding;
//		DirectX::XMFLOAT4 color;
//
//	public:
//		static const MCFontRenderOptions DEFAULT_WHITE() {
//			static MCFontRenderOptions default_white {
//				DirectX::XMFLOAT2 {1.0f, 1.0f},
//				DirectX::XMFLOAT2 {1.0f, 1.0f},
//				DirectX::XMFLOAT4 {1.0f, 1.0f, 1.0f, 1.0f}
//			};
//			return default_white;
//		};
//
//		static const MCFontRenderOptions DEFAULT_WHITE() {
//			static MCFontRenderOptions default_black {
//				DirectX::XMFLOAT2{ 1.0f, 1.0f },
//				DirectX::XMFLOAT2{ 1.0f, 1.0f },
//				DirectX::XMFLOAT4{ 0.0f, 0.0f, 0.0f, 1.0f }
//			};
//			return default_white;
//		};
//	};
//
//	class MCFontRenderer {
//	public:
//		MCFontRenderer();
//		MCFontRenderer(MCFontRenderer&)            = delete;
//		MCFontRenderer& operator=(MCFontRenderer&) = delete;
//		MCFontRenderer(MCFontRenderer&&)           = delete;
//		~MCFontRenderer();
//
//	public:
//		void Render(const char* pText, const MCFont *pFont, const DirectX::XMFLOAT2 &pos, const MCFontRenderOptions &renderOptions);
//
//	private:
//		std::unique_ptr<MCFont>           _pFont;
//		ComPtr<ID3D12GraphicsCommandList> _pCommandList;
//		ComPtr<ID3D12PipelineState>       _pPSO;
//	};
//}
