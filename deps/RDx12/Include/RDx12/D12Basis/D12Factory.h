#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>

namespace RDx12
{
	class D12Factory
	{
	public:
		
		D12Factory();
		IDXGIFactory5* GetFactory();

	private:
		// Enable the debug layer (requires the Graphics Tools "optional feature").
		// NOTE: Enabling the debug layer after device creation will invalidate the active device.
		//要使用此API启用调试层，必须在创建D3D12设备之前调用它。 创建D3D12设备后调用此API将导致D3D12运行时删除该设备。
		//https://docs.microsoft.com/en-us/windows/win32/api/d3d12sdklayers/nf-d3d12sdklayers-id3d12debug-enabledebuglayer
		void EnableDebug();

		UINT nDXGIFactoryFlags = 0;
		Microsoft::WRL::ComPtr<IDXGIFactory5>  compFactory;
	};
}
