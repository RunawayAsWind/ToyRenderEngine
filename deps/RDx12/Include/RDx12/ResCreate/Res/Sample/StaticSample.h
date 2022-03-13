#pragma once
#include <array>
#include "../../../../../3rdParty/Dx12Dragon/d3dx12.h"

namespace RDx12
{
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();
}