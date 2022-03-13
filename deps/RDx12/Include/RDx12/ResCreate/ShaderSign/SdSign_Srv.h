#pragma once
#include "../3rdParty/Dx12Dragon/d3dx12.h"
#include "../DescHeap/HeapManager.h"
#include "../../FrameResource/FrVec.h"

namespace RDx12
{
	/*Srv签名，用于设计ShaderResource在Shader寄存器、根签名中的布局*/
	class SdSign_Srv
	{
	private:
		std::string name;
		UINT rgIdx;
		UINT spIdx;
		UINT desNum;
		UINT rpIdx;
		CD3DX12_DESCRIPTOR_RANGE desRange;
	public:
		
		SdSign_Srv(std::string name, UINT rgIdx, UINT spIdx, UINT desNum = 1) :name(name), rgIdx(rgIdx), spIdx(spIdx), desNum(desNum) {}

		//|SetAsRootSrv -> SetGraphicsRootShaderResourceView|
		void SetAsRootSrv(std::vector<CD3DX12_ROOT_PARAMETER>& vec)
		{
			vec.emplace_back(CD3DX12_ROOT_PARAMETER());
			vec.back().InitAsShaderResourceView(rgIdx, spIdx);
			rpIdx = vec.size() - 1;
		}

		//| SetAsTable -> Bind-> -> CopyDescriptor -> SetGraphicsRootDescriptorTable |
		void SetAsTable(std::vector<CD3DX12_ROOT_PARAMETER>& vec)
		{
			vec.emplace_back(CD3DX12_ROOT_PARAMETER());
			desRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, desNum, rgIdx, spIdx);
			vec.back().InitAsDescriptorTable(1, &desRange);
			rpIdx = vec.size() - 1;
		}

		constexpr UINT RgIdx() const noexcept { return rgIdx; }
		constexpr UINT SpIdx() const noexcept { return spIdx; }
		constexpr UINT DesNum() const noexcept { return desNum; }
		constexpr UINT RpIdx() const noexcept { return rpIdx; }
		std::string Name() const noexcept { return name; }
	};
}