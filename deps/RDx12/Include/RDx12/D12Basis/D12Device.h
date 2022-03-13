#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>

namespace RDx12
{
	class D12Device
	{
	public:
		D12Device(IDXGIFactory5* factory, D3D_FEATURE_LEVEL level = D3D_FEATURE_LEVEL_11_1, UINT startIdx = 0, bool useWarpDevice = false);
		ID3D12Device4* GetDev();

	private:
		Microsoft::WRL::ComPtr<ID3D12Device4> comDev;
	};
}