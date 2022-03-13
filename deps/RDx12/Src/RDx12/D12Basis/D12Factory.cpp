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
				nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;//（4）多个文件公用一个变量的方法：1.在头文件里面声明，2.在头文件对应的cpp里定义，3.在include本头文件的文件全局区加上extern UINT nDXGIFactoryFlags，4.在函数里修改值
			}
		}
#endif
	}

	D12Factory::D12Factory()
	{
		EnableDebug();
		RWin32::ThrowIfFailed(CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(&compFactory)),L"DXGIFactory创建成功\n",L"DXGIFactory创建失败！！！！！！！！！！\n");
	}

	IDXGIFactory5* D12Factory::GetFactory()
	{
		return compFactory.Get();
	}
}