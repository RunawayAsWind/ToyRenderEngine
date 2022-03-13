#pragma once
#include "../../../Util/D12Util.h"
#include "../../../../../3rdParty/Dx12Dragon/d3dx12.h"

namespace RDx12
{
	class Res_SingleTypeBuffer
	{
	public:
		Res_SingleTypeBuffer(UINT typeSize, bool isConstantBuffer);
		D3D12_GPU_VIRTUAL_ADDRESS aGpu(UINT elementIndex);
		D3D12_INDEX_BUFFER_VIEW IndexBufferView();
		D3D12_VERTEX_BUFFER_VIEW VertexBufferView();
		D3D12_CONSTANT_BUFFER_VIEW_DESC CbViewDesc(UINT idx);
		constexpr UINT MaxNum() noexcept
		{ return bufferSize / bufferStride; }
		ID3D12Resource* GetRes(){ return res.Get(); }

		virtual void CreateByMaxNum(UINT maxNum, ID3D12Device4* dev) {};
		virtual void CopyData(int eleidx, void* data) {};
		virtual void SetMap(UINT subResIndex = 0, D3D12_RANGE* readRange = nullptr) {};
		virtual void Unmap(UINT subResIndex = 0, D3D12_RANGE* readRange = nullptr) {};
		virtual void UpdateRes(ID3D12GraphicsCommandList* cmdList) {};
	protected:
		Microsoft::WRL::ComPtr<ID3D12Resource> res;
		UINT bufferStride = 0;
		UINT bufferSize = 0;
		bool isCb = false;
		DXGI_FORMAT IndexFormat = DXGI_FORMAT_R32_UINT;
	};
}