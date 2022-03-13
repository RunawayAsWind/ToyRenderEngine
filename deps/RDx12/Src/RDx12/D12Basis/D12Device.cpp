#include "../Include/RDx12/D12Basis/D12Device.h"

#include <RWin32/Win32Debug.h>

namespace RDx12
{
	D12Device::D12Device(IDXGIFactory5* factory, D3D_FEATURE_LEVEL level, UINT startIdx, bool useWarpDevice)
	{
		//WARP Render是dx提供的一种的高性能软渲染组件，需要支持WARP功能的软件驱动才能进行WARP渲染
		// https://docs.microsoft.com/en-us/windows/win32/direct3darticles/directx-warp

		if (useWarpDevice)//Provides an adapter which can be provided to D3D12CreateDevice to use the WARP renderer.
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
			factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
			RWin32::ThrowIfFailed(D3D12CreateDevice(adapter.Get(), level, IID_PPV_ARGS(&comDev)),L"device创建成功\n",L"device创建失败！！！！\n");
		}
		else//Enumerates both adapters (video cards) with or without outputs.
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
			//枚举适配器，并选择合适的适配器来创建3D设备对象 //如果查找不到合适的硬件虚拟适配器，请调整初始下标adapterIndex
			for (UINT adapterIndex = startIdx; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				WIN32DEBUG(L"当前适配器:VendorId：0x%x    SubSysId 0x%x   Flags:%d    \n", desc.VendorId, desc.SubSysId, desc.Flags);

				//跳过软件虚拟适配器设备
				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)//https://docs.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_adapter_flag
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					WIN32DEBUG(L"是软件适配器，跳过\n");
					continue;
				}

				// Check to see if the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				//最后一个参数为null，防止真正的创建设备
				//D3D_FEATURE_LEVEL_11_0是版本，设备是否满足可以通过cmd的dxdiag，驱动程序-功能级别查看，程序内部会从最高版本向下搜索，知道合适
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), level, _uuidof(ID3D12Device), nullptr)))
				{
					WIN32DEBUG(L"是硬件适配器，记录\n");
					break;
				}
			}

			RWin32::ThrowIfFailed(D3D12CreateDevice(adapter.Get(), level, IID_PPV_ARGS(&comDev)),L"device创建成功\n",L"device创建失败！！！！\n");
		}
	}

	ID3D12Device4* D12Device::GetDev()
	{
		return comDev.Get();
	}
}
