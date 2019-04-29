#pragma once

#include "MCPrimitives.h"

namespace MC {

	typedef void* MCHANDLE;

	struct MCANAMATION_FRAME_SET {
		unsigned short StartFrame;
		unsigned short EndFrame;
	};

	struct MCSPRITE_DESC {
		MCHANDLE       hTexture;	// Handle to graphics texture atlas.
		unsigned short FrameWith;   // The width of a frame in the texture atlas.
		unsigned short FrameHeight; // The height of the frame in the texture atlas.
		unsigned char  FramesX;     // The number of frames width-wise in the texture atlas.
		unsigned char  FramesY;     // The number of frames height-wise in the texture atlas.
		unsigned char  AnimationFrameSetCount;      // The number of animation frame sets.
		MCANAMATION_FRAME_SET *pAnimationFrameSets; // Pointer to N AnimationFrameSets.
	};

	typedef enum MCIMAGE_FILE_FORMAT : unsigned char {
		MCIMAGE_FILE_FORMAT_INVALID = 0,
		MCIMAGE_FILE_FORMAT_BITMAP = 1,
		MCIMAGE_FILE_FORMAT_DDS = 2,
		MCIMAGE_FILE_FORMAT_PNG = 3
	} MCIMAGE_FILE_FORMAT;

	struct MCTEXTURE_DESC {
		MCIMAGE_FILE_FORMAT ImageFormat;
		const char        *pFilename;
		unsigned short      Width;
		unsigned short      Height;
		unsigned char       BitDepth;
		bool                Alpha;
	};

	typedef enum class MCMESH_FORMAT {
		INVALID        = 0,
		TRIANGLE_STRIP = 1,
		TRIANGLE_LIST  = 2
	} MCMESH_FORMAT;

	struct MCMESH_DESC {
		MCMESH_FORMAT   MeshFormat;
		MCVERTEX_FORMAT VertexFormat;
		void*           pVertexData;
		unsigned int    VertexBufferByteSize;
		void*           pIndexData;
		MCBIT_DEPTH     IndexBitDepth;
		unsigned int    IndexBufferByteSize;
	};

	typedef enum class MCGEOMETRY_MESH_TYPE {
		CUBE	= 0,
		SPHERE  = 1
	} MCGEOMETRY_MESH_TYPE;

	struct MCGEOMETRY_MESH_DESC {
		MCGEOMETRY_MESH_TYPE Type;
		DirectX::XMFLOAT3    Position;
		DirectX::XMFLOAT3    Scale;  // Use Scale.x for a radius
		unsigned int         NumSubdivisions; // Use when applicable
	};

}