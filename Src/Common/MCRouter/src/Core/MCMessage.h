#pragma once

#include <stdint.h>

namespace MC {

	typedef enum MC_MESSAGE_VISIBILITY : uint8_t {
		MC_MESSAGE_VISIBILITY_NONE    = 0x00,
		MC_MESSAGE_VISIBILITY_RENDER  = 0x01,
		MC_MESSAGE_VISIBILITY_AUDIO   = 0x02,
		MC_MESSAGE_VISIBILITY_GAME    = 0x04,
		MC_MESSAGE_VISIBILITY_CONSOLE = 0x08,
		MC_MESSAGE_VISIBILITY_ALL     = 0xFF
	} MC_MESSAGE_VISIBILITY;

	typedef enum MC_MESSAGE_FLAGS : uint8_t {
		MC_MESSAGE_FLAGS_NONE = 0x00
	} MC_MESSAGE_FLAGS;

	typedef enum MC_MESSAGE_BIT_FLAGS : uint16_t {
		MC_MESSAGE_BIT_FLAGS_NONE      = 0x0000,
		MC_MESSAGE_BIT_FLAGS_32_BIT    = 0x1000,
		MC_MESSAGE_BIT_FLAGS_64_BIT    = 0x2000,
		MC_MESSAGE_BIT_FLAGS_128_BIT   = 0x4000,
		MC_MESSAGE_BIT_FLAGS_ADDRESSED = 0x8000
	};

	typedef enum McMESSAGE : uint16_t {
		MC_MESSAGE_INVALID                     = 0x0000,
			/* Marker for 16-bit messages. */

			/* KEY BOARD MESSAGES ----------------------------------------------------------------------- */
				
		MC_MESSAGE_KEY_DOWN_32                 = 0x0001 | MC_MESSAGE_BIT_FLAGS_32_BIT,
			/* Key down message 
				Visibility: MC_MESSAGE_VISIBILITY_GAME | MC_MESSAGE_VISIBILITY_CONSOLE 
				Flags: Not used.
				Param: Key Code of the pressed key.
			*/
		MC_MESSAGE_KEY_UP_32                   = 0x0002 | MC_MESSAGE_BIT_FLAGS_32_BIT,
			/* Key up message
				Visibility: MC_MESSAGE_VISIBILITY_GAME | MC_MESSAGE_VISIBILITY_CONSOLE
				Flags: Not used.
				Param: Key Code of the pressed key.
			*/

		MC_MESSAGE_SET_RENDERER_STATE_32       = 0x0003 | MC_MESSAGE_BIT_FLAGS_32_BIT,
			/* Set the renderer state.
				Visibility: Usually MC_MESSAGE_VISIBILITY_RENDER
				Flags: not used.
				Param: The target render state. The renderer will throw if Param !(MC_RENDERER_STATE). */

		MC_MESSAGE_FRAME_READY_128             = 0x0001 | MC_MESSAGE_BIT_FLAGS_128_BIT | MC_MESSAGE_BIT_FLAGS_ADDRESSED
			/* The game logic has a new frame. 
				Visibility:       MC_MESSAGE_VISIBILITY_RENDERER 
				Flags:            not used.
				LOParam32:    not used.
				pAddress:      the address of the frame.
				LOParam16HI: not used. */

	} MC_MESSAGE;

	typedef unsigned short MC_MESSAGE_ADDRESS;

#pragma pack(push, 1)
	struct alignas(4) MC_MESSAGE32 {
		MC_MESSAGE            Message;
		MC_MESSAGE_VISIBILITY Visibility;
		union {
			MC_MESSAGE_FLAGS  Flags;
			uint8_t           Param;
		};
	};
	static_assert(sizeof(MC_MESSAGE32) == 4, "sizeof(MC_MESSAGE32) should be 4 bytes!!!");

	struct alignas(8) MC_MESSAGE64 {
		MC_MESSAGE            Message;
		MC_MESSAGE_VISIBILITY Visibility;
		MC_MESSAGE_FLAGS      Flags;
		union {
			uint32_t		  LOParam32;
			struct {
				uint16_t      LOParam16LO;
				uint16_t      LOParam16HI;
			};
		};
	};

	static_assert(sizeof(MC_MESSAGE64) == 8, "sizeof(MC_MESSAGE64) should be 8 bytes!!!");

	struct alignas(16) MC_MESSAGE128 {		
		MC_MESSAGE			  Message;
		MC_MESSAGE_VISIBILITY Visibility;
		MC_MESSAGE_FLAGS	  Flags;
		union {
			uint32_t		  LOParam32;
			struct {
				union {
					uint16_t  LOParam16LO;
				};
				uint16_t      LOParam16HI;
			};
		};
		union {
			char*             pAddress;
			uint64_t		  HIParam64;
			struct {
				uint32_t      HIParam32LO;
				uint32_t      HIParam32HI;
			};
		};
	};
#pragma pack(pop)

	static_assert(sizeof(MC_MESSAGE128) == 16, "sizeof(MC_MESSAGE128) should be 16 bytes!!!");
}
