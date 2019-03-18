//
//#include "MCFontLoader.h"
//
//namespace MC {
//
//	int LoadImageDataFromFile(BYTE** imageData, D3D12_RESOURCE_DESC& resourceDescription, LPCWSTR filename, int &bytesPerRow)
//	{
//		HRESULT hr;
//
//		// we only need one instance of the imaging factory to create decoders and frames
//		static IWICImagingFactory *wicFactory;
//
//		// reset decoder, frame and converter since these will be different for each image we load
//		IWICBitmapDecoder *wicDecoder = NULL;
//		IWICBitmapFrameDecode *wicFrame = NULL;
//		IWICFormatConverter *wicConverter = NULL;
//
//		bool imageConverted = false;
//
//		if (wicFactory == NULL)
//		{
//			// Initialize the COM library
//			CoInitialize(NULL);
//
//			// create the WIC factory
//			hr = CoCreateInstance(
//				CLSID_WICImagingFactory,
//				NULL,
//				CLSCTX_INPROC_SERVER,
//				IID_PPV_ARGS(&wicFactory)
//			);
//			if (FAILED(hr)) return 0;
//		}
//
//		// load a decoder for the image
//		hr = wicFactory->CreateDecoderFromFilename(
//			filename,                        // Image we want to load in
//			NULL,                            // This is a vendor ID, we do not prefer a specific one so set to null
//			GENERIC_READ,                    // We want to read from this file
//			WICDecodeMetadataCacheOnLoad,    // We will cache the metadata right away, rather than when needed, which might be unknown
//			&wicDecoder                      // the wic decoder to be created
//		);
//		if (FAILED(hr)) return 0;
//
//		// get image from decoder (this will decode the "frame")
//		hr = wicDecoder->GetFrame(0, &wicFrame);
//		if (FAILED(hr)) return 0;
//
//		// get wic pixel format of image
//		WICPixelFormatGUID pixelFormat;
//		hr = wicFrame->GetPixelFormat(&pixelFormat);
//		if (FAILED(hr)) return 0;
//
//		// get size of image
//		UINT textureWidth, textureHeight;
//		hr = wicFrame->GetSize(&textureWidth, &textureHeight);
//		if (FAILED(hr)) return 0;
//
//		// we are not handling sRGB types in this tutorial, so if you need that support, you'll have to figure
//		// out how to implement the support yourself
//
//		// convert wic pixel format to dxgi pixel format
//		DXGI_FORMAT dxgiFormat = GetDXGIFormatFromWICFormat(pixelFormat);
//
//		// if the format of the image is not a supported dxgi format, try to convert it
//		if (dxgiFormat == DXGI_FORMAT_UNKNOWN)
//		{
//			// get a dxgi compatible wic format from the current image format
//			WICPixelFormatGUID convertToPixelFormat = GetConvertToWICFormat(pixelFormat);
//
//			// return if no dxgi compatible format was found
//			if (convertToPixelFormat == GUID_WICPixelFormatDontCare) return 0;
//
//			// set the dxgi format
//			dxgiFormat = GetDXGIFormatFromWICFormat(convertToPixelFormat);
//
//			// create the format converter
//			hr = wicFactory->CreateFormatConverter(&wicConverter);
//			if (FAILED(hr)) return 0;
//
//			// make sure we can convert to the dxgi compatible format
//			BOOL canConvert = FALSE;
//			hr = wicConverter->CanConvert(pixelFormat, convertToPixelFormat, &canConvert);
//			if (FAILED(hr) || !canConvert) return 0;
//
//			// do the conversion (wicConverter will contain the converted image)
//			hr = wicConverter->Initialize(wicFrame, convertToPixelFormat, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom);
//			if (FAILED(hr)) return 0;
//
//			// this is so we know to get the image data from the wicConverter (otherwise we will get from wicFrame)
//			imageConverted = true;
//		}
//
//		int bitsPerPixel = GetDXGIFormatBitsPerPixel(dxgiFormat); // number of bits per pixel
//		bytesPerRow = (textureWidth * bitsPerPixel) / 8; // number of bytes in each row of the image data
//		int imageSize = bytesPerRow * textureHeight; // total image size in bytes
//
//													 // allocate enough memory for the raw image data, and set imageData to point to that memory
//		*imageData = (BYTE*)malloc(imageSize);
//
//		// copy (decoded) raw image data into the newly allocated memory (imageData)
//		if (imageConverted)
//		{
//			// if image format needed to be converted, the wic converter will contain the converted image
//			hr = wicConverter->CopyPixels(0, bytesPerRow, imageSize, *imageData);
//			if (FAILED(hr)) return 0;
//		}
//		else
//		{
//			// no need to convert, just copy data from the wic frame
//			hr = wicFrame->CopyPixels(0, bytesPerRow, imageSize, *imageData);
//			if (FAILED(hr)) return 0;
//		}
//
//		// now describe the texture with the information we have obtained from the image
//		resourceDescription = {};
//		resourceDescription.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//		resourceDescription.Alignment = 0; // may be 0, 4KB, 64KB, or 4MB. 0 will let runtime decide between 64KB and 4MB (4MB for multi-sampled textures)
//		resourceDescription.Width = textureWidth; // width of the texture
//		resourceDescription.Height = textureHeight; // height of the texture
//		resourceDescription.DepthOrArraySize = 1; // if 3d image, depth of 3d image. Otherwise an array of 1D or 2D textures (we only have one image, so we set 1)
//		resourceDescription.MipLevels = 1; // Number of mipmaps. We are not generating mipmaps for this texture, so we have only one level
//		resourceDescription.Format = dxgiFormat; // This is the dxgi format of the image (format of the pixels)
//		resourceDescription.SampleDesc.Count = 1; // This is the number of samples per pixel, we just want 1 sample
//		resourceDescription.SampleDesc.Quality = 0; // The quality level of the samples. Higher is better quality, but worse performance
//		resourceDescription.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN; // The arrangement of the pixels. Setting to unknown lets the driver choose the most efficient one
//		resourceDescription.Flags = D3D12_RESOURCE_FLAG_NONE; // no flags
//
//															  // return the size of the image. remember to delete the image once your done with it (in this tutorial once its uploaded to the gpu)
//		return imageSize;
//	}
//
//	Font LoadFont(LPCWSTR filename, int windowWidth, int windowHeight)
//	{
//		std::wifstream fs;
//		fs.open(filename);
//
//		Font font;
//		std::wstring tmp;
//		int startpos;
//
//		// extract font name
//		fs >> tmp >> tmp; // info face="Arial"
//		startpos = tmp.find(L""") + 1;
//			font.name = tmp.substr(startpos, tmp.size() - startpos - 1);
//
//		// get font size
//		fs >> tmp; // size=73
//		startpos = tmp.find(L"=") + 1;
//		font.size = std::stoi(tmp.substr(startpos, tmp.size() - startpos));
//
//		// bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing
//		fs >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp; // bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 
//
//															 // get padding
//		fs >> tmp; // padding=5,5,5,5 
//		startpos = tmp.find(L"=") + 1;
//		tmp = tmp.substr(startpos, tmp.size() - startpos); // 5,5,5,5
//
//														   // get up padding
//		startpos = tmp.find(L",") + 1;
//		font.toppadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;
//
//		// get right padding
//		tmp = tmp.substr(startpos, tmp.size() - startpos);
//		startpos = tmp.find(L",") + 1;
//		font.rightpadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;
//
//		// get down padding
//		tmp = tmp.substr(startpos, tmp.size() - startpos);
//		startpos = tmp.find(L",") + 1;
//		font.bottompadding = std::stoi(tmp.substr(0, startpos)) / (float)windowWidth;
//
//		// get left padding
//		tmp = tmp.substr(startpos, tmp.size() - startpos);
//		font.leftpadding = std::stoi(tmp) / (float)windowWidth;
//
//		fs >> tmp; // spacing=0,0
//
//				   // get lineheight (how much to move down for each line), and normalize (between 0.0 and 1.0 based on size of font)
//		fs >> tmp >> tmp; // common lineHeight=95
//		startpos = tmp.find(L"=") + 1;
//		font.lineHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;
//
//		// get base height (height of all characters), and normalize (between 0.0 and 1.0 based on size of font)
//		fs >> tmp; // base=68
//		startpos = tmp.find(L"=") + 1;
//		font.baseHeight = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;
//
//		// get texture width
//		fs >> tmp; // scaleW=512
//		startpos = tmp.find(L"=") + 1;
//		font.textureWidth = std::stoi(tmp.substr(startpos, tmp.size() - startpos));
//
//		// get texture height
//		fs >> tmp; // scaleH=512
//		startpos = tmp.find(L"=") + 1;
//		font.textureHeight = std::stoi(tmp.substr(startpos, tmp.size() - startpos));
//
//		// get pages, packed, page id
//		fs >> tmp >> tmp; // pages=1 packed=0
//		fs >> tmp >> tmp; // page id=0
//
//						  // get texture filename
//		std::wstring wtmp;
//		fs >> wtmp; // file="Arial.png"
//		startpos = wtmp.find(L""") + 1;
//			font.fontImage = wtmp.substr(startpos, wtmp.size() - startpos - 1);
//
//		// get number of characters
//		fs >> tmp >> tmp; // chars count=97
//		startpos = tmp.find(L"=") + 1;
//		font.numCharacters = std::stoi(tmp.substr(startpos, tmp.size() - startpos));
//
//		// initialize the character list
//		font.CharList = new FontChar[font.numCharacters];
//
//		for (int c = 0; c < font.numCharacters; ++c)
//		{
//			// get unicode id
//			fs >> tmp >> tmp; // char id=0
//			startpos = tmp.find(L"=") + 1;
//			font.CharList[c].id = std::stoi(tmp.substr(startpos, tmp.size() - startpos));
//
//			// get x
//			fs >> tmp; // x=392
//			startpos = tmp.find(L"=") + 1;
//			font.CharList[c].u = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)font.textureWidth;
//
//			// get y
//			fs >> tmp; // y=340
//			startpos = tmp.find(L"=") + 1;
//			font.CharList[c].v = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)font.textureHeight;
//
//			// get width
//			fs >> tmp; // width=47
//			startpos = tmp.find(L"=") + 1;
//			tmp = tmp.substr(startpos, tmp.size() - startpos);
//			font.CharList[c].width = (float)std::stoi(tmp) / (float)windowWidth;
//			font.CharList[c].twidth = (float)std::stoi(tmp) / (float)font.textureWidth;
//
//			// get height
//			fs >> tmp; // height=57
//			startpos = tmp.find(L"=") + 1;
//			tmp = tmp.substr(startpos, tmp.size() - startpos);
//			font.CharList[c].height = (float)std::stoi(tmp) / (float)windowHeight;
//			font.CharList[c].theight = (float)std::stoi(tmp) / (float)font.textureHeight;
//
//			// get xoffset
//			fs >> tmp; // xoffset=-6
//			startpos = tmp.find(L"=") + 1;
//			font.CharList[c].xoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;
//
//			// get yoffset
//			fs >> tmp; // yoffset=16
//			startpos = tmp.find(L"=") + 1;
//			font.CharList[c].yoffset = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowHeight;
//
//			// get xadvance
//			fs >> tmp; // xadvance=65
//			startpos = tmp.find(L"=") + 1;
//			font.CharList[c].xadvance = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos)) / (float)windowWidth;
//
//			// get page
//			// get channel
//			fs >> tmp >> tmp; // page=0    chnl=0
//		}
//
//		// get number of kernings
//		fs >> tmp >> tmp; // kernings count=96
//		startpos = tmp.find(L"=") + 1;
//		font.numKernings = std::stoi(tmp.substr(startpos, tmp.size() - startpos));
//
//		// initialize the kernings list
//		font.KerningsList = new FontKerning[font.numKernings];
//
//		for (int k = 0; k < font.numKernings; ++k)
//		{
//			// get first character
//			fs >> tmp >> tmp; // kerning first=87
//			startpos = tmp.find(L"=") + 1;
//			font.KerningsList[k].firstid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));
//
//			// get second character
//			fs >> tmp; // second=45
//			startpos = tmp.find(L"=") + 1;
//			font.KerningsList[k].secondid = std::stoi(tmp.substr(startpos, tmp.size() - startpos));
//
//			// get amount
//			fs >> tmp; // amount=-1
//			startpos = tmp.find(L"=") + 1;
//			int t = (float)std::stoi(tmp.substr(startpos, tmp.size() - startpos));
//			font.KerningsList[k].amount = (float)t / (float)windowWidth;
//		}
//
//		return font;
//	}
//
//	
//
//}