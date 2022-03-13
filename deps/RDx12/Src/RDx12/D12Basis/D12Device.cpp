#include "../Include/RDx12/D12Basis/D12Device.h"

#include <RWin32/Win32Debug.h>

namespace RDx12
{
	D12Device::D12Device(IDXGIFactory5* factory, D3D_FEATURE_LEVEL level, UINT startIdx, bool useWarpDevice)
	{
		//WARP Render��dx�ṩ��һ�ֵĸ���������Ⱦ�������Ҫ֧��WARP���ܵ�����������ܽ���WARP��Ⱦ
		// https://docs.microsoft.com/en-us/windows/win32/direct3darticles/directx-warp

		if (useWarpDevice)//Provides an adapter which can be provided to D3D12CreateDevice to use the WARP renderer.
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
			factory->EnumWarpAdapter(IID_PPV_ARGS(&adapter));
			RWin32::ThrowIfFailed(D3D12CreateDevice(adapter.Get(), level, IID_PPV_ARGS(&comDev)),L"device�����ɹ�\n",L"device����ʧ�ܣ�������\n");
		}
		else//Enumerates both adapters (video cards) with or without outputs.
		{
			Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
			//ö������������ѡ����ʵ�������������3D�豸���� //������Ҳ������ʵ�Ӳ���������������������ʼ�±�adapterIndex
			for (UINT adapterIndex = startIdx; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &adapter); ++adapterIndex)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				WIN32DEBUG(L"��ǰ������:VendorId��0x%x    SubSysId 0x%x   Flags:%d    \n", desc.VendorId, desc.SubSysId, desc.Flags);

				//������������������豸
				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)//https://docs.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_adapter_flag
				{
					// Don't select the Basic Render Driver adapter.
					// If you want a software adapter, pass in "/warp" on the command line.
					WIN32DEBUG(L"�����������������\n");
					continue;
				}

				// Check to see if the adapter supports Direct3D 12, but don't create the
				// actual device yet.
				//���һ������Ϊnull����ֹ�����Ĵ����豸
				//D3D_FEATURE_LEVEL_11_0�ǰ汾���豸�Ƿ��������ͨ��cmd��dxdiag����������-���ܼ���鿴�������ڲ������߰汾����������֪������
				if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), level, _uuidof(ID3D12Device), nullptr)))
				{
					WIN32DEBUG(L"��Ӳ������������¼\n");
					break;
				}
			}

			RWin32::ThrowIfFailed(D3D12CreateDevice(adapter.Get(), level, IID_PPV_ARGS(&comDev)),L"device�����ɹ�\n",L"device����ʧ�ܣ�������\n");
		}
	}

	ID3D12Device4* D12Device::GetDev()
	{
		return comDev.Get();
	}
}
