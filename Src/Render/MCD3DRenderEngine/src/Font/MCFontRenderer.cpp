//#include "../Headers/MCFontRenderer.h"
//
//namespace MC {
//
//
//	void MCFontRenderer::Render(const char* pText, const MCFont *pFont, const DirectX::XMFLOAT2 &pos, const MCFontRenderOptions &renderOptions) {
//		// clear the depth buffer so we can draw over everything
//		commandList->ClearDepthStencilView(dsDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
//
//		// set the text pipeline state object
//		commandList->SetPipelineState(textPSO);
//
//		// this way we only need 4 vertices per quad rather than 6 if we were to use a triangle list topology
//		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//
//		// set the text vertex buffer
//		commandList->IASetVertexBuffers(0, 1, &textVertexBufferView[frameIndex]);
//
//		// bind the text srv. We will assume the correct descriptor heap and table are currently bound and set
//		commandList->SetGraphicsRootDescriptorTable(1, font.srvHandle);
//
//		int numCharacters = 0;
//
//		float topLeftScreenX = (pos.x * 2.0f) - 1.0f;
//		float topLeftScreenY = ((1.0f - pos.y) * 2.0f) - 1.0f;
//
//		float x = topLeftScreenX;
//		float y = topLeftScreenY;
//
//		float horrizontalPadding = (font.leftpadding + font.rightpadding) * padding.x;
//		float verticalPadding = (font.toppadding + font.bottompadding) * padding.y;
//
//		// cast the gpu virtual address to a textvertex, so we can directly store our vertices there
//		TextVertex* vert = (TextVertex*)textVBGPUAddress[frameIndex];
//
//		wchar_t lastChar = -1; // no last character to start with
//
//		for (int i = 0; i < text.size(); ++i)
//		{
//			wchar_t c = text[i];
//
//			FontChar* fc = font.GetChar(c);
//
//			// character not in font char set
//			if (fc == nullptr)
//				continue;
//
//			// end of string
//			if (c == L'')
//				break;
//
//			// new line
//			if (c == L'n')
//			{
//				x = topLeftScreenX;
//				y -= (font.lineHeight + verticalPadding) * scale.y;
//				continue;
//			}
//
//			// don't overflow the buffer. In your app if this is true, you can implement a resize of your text vertex buffer
//			if (numCharacters >= maxNumTextCharacters)
//				break;
//
//			float kerning = 0.0f;
//			if (i > 0)
//				kerning = font.GetKerning(lastChar, c);
//
//			vert[numCharacters] = TextVertex(color.x,
//				color.y,
//				color.z,
//				color.w,
//				fc->u,
//				fc->v,
//				fc->twidth,
//				fc->theight,
//				x + ((fc->xoffset + kerning) * scale.x),
//				y - (fc->yoffset * scale.y),
//				fc->width * scale.x,
//				fc->height * scale.y);
//
//			numCharacters++;
//
//			// remove horrizontal padding and advance to next char position
//			x += (fc->xadvance - horrizontalPadding) * scale.x;
//
//			lastChar = c;
//		}
//
//		// we are going to have 4 vertices per character (trianglestrip to make quad), and each instance is one character
//		commandList->DrawInstanced(4, numCharacters, 0, 0);
//	}
//
//}