#include "../Include/RDx12/D12Basis/D12Factory.h"

#include <RWin32/Win32Debug.h>

namespace RDx12
{
	void D12Factory::EnableDebug()
	{
#if defined(_DEBUG)
		{
			nDXGIFactoryFlags = 0;
			Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();

				// Enable additional debug layers.
				nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;//��4������ļ�����һ�������ķ�����1.��ͷ�ļ�����������2.��ͷ�ļ���Ӧ��cpp�ﶨ�壬3.��include��ͷ�ļ����ļ�ȫ��������extern UINT nDXGIFactoryFlags��4.�ں������޸�ֵ
			}
		}
#endif
	}

	D12Factory::D12Factory()
	{
		EnableDebug();
		RWin32::ThrowIfFailed(CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&compFactory)),L"DXGIFactory�����ɹ�\n",L"DXGIFactory����ʧ�ܣ�������������������\n");
	}

	IDXGIFactory5* D12Factory::GetFactory()
	{
		return compFactory.Get();
	}
}