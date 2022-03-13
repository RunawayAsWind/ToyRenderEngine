#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>

//linker
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "D12Factory.h"
#include "D12Device.h"
#include "D12CmdQue.h"

namespace RDx12
{
	class D12Basis
	{
	public:
		D12Factory baseFactory;
		D12Device baseDev;
		D12CmdQue baseCmdQue;

		D12Basis::D12Basis() :baseDev{ baseFactory.GetFactory() }, baseCmdQue{ baseDev.GetDev() } {}
		IDXGIFactory5* GetFactory(){return baseFactory.GetFactory();}
		ID3D12Device4* GetDevice(){return baseDev.GetDev();}
	};
}
