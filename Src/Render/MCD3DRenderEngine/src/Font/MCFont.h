//#pragma once
//
//#include "../Core/MCD3D12Core.h"
//
//namespace MC {
//
//	struct MCFontChar
//	{
//		// the ASCII id of the character.
//		char id;
//
//		// these need to be converted to texture coordinates 
//		// (where 0.0 is 0 and 1.0 is textureWidth of the font)
//		float u; // u texture coordinate
//		float v; // v texture coordinate
//		float twidth; // width of character on texture
//		float theight; // height of character on texture
//
//		float width; // width of character in screen coords
//		float height; // height of character in screen coords
//
//					  // these need to be normalized based on size of font
//		float xoffset; // offset from current cursor pos to left side of character
//		float yoffset; // offset from top of line to top of character
//		float xadvance; // how far to move to right for next character
//	};
//
//	struct MCFontKerning
//	{
//		int firstid; // the first character
//		int secondid; // the second character
//		float amount; // the amount to add/subtract to second characters x
//	};
//
//	struct MCFont
//	{
//		std::string name;          // name of the font
//		std::string fontImage;     // filename image file.
//		int         size;          // size of font, lineheight and baseheight will be based on this as if this is a single unit (1.0)
//		float       lineHeight;    // how far to move down to next line, will be normalized
//		float       baseHeight;    // height of all characters, will be normalized
//		int         textureWidth;  // width of the font texture
//		int         textureHeight; // height of the font texture
//		int         numCharacters; // number of characters in the font
//		MCFontChar* CharList;      // list of characters
//		int         numKernings;   // the number of kernings
//		MCFontKerning*  KerningsList;  // list to hold kerning values
//		ID3D12Resource* textureBuffer; // the font texture resource
//		D3D12_GPU_DESCRIPTOR_HANDLE srvHandle; // the font srv
//
//											   // these are how much the character is padded in the texture. We
//											   // add padding to give sampling a little space so it does not accidentally
//											   // padd the surrounding characters. We will need to subtract these paddings
//											   // from the actual spacing between characters to remove the gaps you would otherwise see
//		float leftpadding;
//		float toppadding;
//		float rightpadding;
//		float bottompadding;
//
//		// this will return the amount of kerning we need to use for two characters
//		float GetKerning(wchar_t first, wchar_t second)
//		{
//			for (int i = 0; i < numKernings; ++i)
//			{
//				if ((wchar_t)KerningsList[i].firstid == first && (wchar_t)KerningsList[i].secondid == second)
//					return KerningsList[i].amount;
//			}
//			return 0.0f;
//		}
//
//		// this will return a FontChar given a wide character
//		MCFontChar* GetChar(wchar_t c)
//		{
//			for (int i = 0; i < numCharacters; ++i)
//			{
//				if (c == (wchar_t)CharList[i].id)
//					return &CharList[i];
//			}
//			return nullptr;
//		}
//	};
//
//}