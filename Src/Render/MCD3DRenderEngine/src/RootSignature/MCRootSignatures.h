#pragma once

#include "../Core/MCD3D12Core.h"

using Microsoft::WRL::ComPtr;

namespace MC { namespace RootSignatures {

	/*
	                 		DEFAULT  ROOT  SIGNATURE

		+----------------------------+----+
		|   |                        |    |
		| 0 |        ROOT CBV        | B0 |  --> Matrix4x4 --> Object location
		+---+------------------------+----|              +--------------+----+	
		| 1 |       DESC_TABLE            |  ----------> |      CBV     | B1 | --> Frame Constants	 
		+---+-----------------------------+              +--------------+----+

		+------------------+----+
		|  STATIC SAMPLER  | S0 |
		+------------------+----+
		|  STATIC SAMPLER  | S1 |
		+------------------+----+
																						
	*/
	ComPtr<ID3D12RootSignature> Default();

} }