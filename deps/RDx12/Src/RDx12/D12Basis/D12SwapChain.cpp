#include <assert.h>
#include <RWin32/Win32Debug.h>
#include "../Include/RDx12/D12Basis/D12SwapChain.h"

namespace RDx12
{
	DXGI_SWAP_CHAIN_DESC1 D12SwapChain::DefaultDesc(UINT backCount, DXGI_FORMAT fmt, UINT sampleCount,ID3D12Device4* dev)
	{
		D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
		msQualityLevels.Format = fmt;
		msQualityLevels.SampleCount = sampleCount;
		msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
		msQualityLevels.NumQualityLevels = 0;
		RWin32::ThrowIfFailed(
			dev->CheckFeatureSupport(
				D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
				&msQualityLevels,
				sizeof(msQualityLevels))
		);

		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Format = fmt;											//最广泛的数据格式，如何选取格式 https://stackoverflow.com/questions/45636299/what-factors-determine-dxgi-format
		desc.SampleDesc.Count = msQualityLevels.NumQualityLevels > 0? msQualityLevels.SampleCount : 1;	//设置多采样模式，其实只能设置1，实现MSAA等抗锯齿功能还是要先渲染在RTT上，https://stackoverflow.com/questions/40110699/creating-a-swap-chain-with-msaa-fails
		desc.SampleDesc.Quality = msQualityLevels.NumQualityLevels > 0? msQualityLevels.NumQualityLevels-1 : 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;			//一般情况下交换链只需要用于输出就行了，SHADERINPUT可以用RTT
		desc.BufferCount = backCount;								//后缓冲数量
		desc.Scaling = DXGI_SCALING_NONE;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;			//正常情况下一般UWP采用FLIP DISCARD https://docs.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_swap_effect  https://www.cnblogs.com/lc-cnblong/p/3365031.html https://docs.microsoft.com/zh-cn/windows/win32/direct3ddxgi/dxgi-flip-model
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		return desc;
	}

	void D12SwapChain::CreateSwapChain(DXGI_SWAP_CHAIN_DESC1 desc,IDXGIFactory5* factory, ID3D12CommandQueue* cmdQue, HWND hWnd)
	{
		RWin32::ThrowIfFailed
		(
			factory->CreateSwapChainForHwnd
			(
				cmdQue,
				hWnd,
				&desc,
				nullptr,				//关系到交换链是否全屏
				nullptr,				//关系到交换链输出限制
				&compSch1
			),
			L"交换链创建成功\n",
			L"交换链创建失败！！！！\n"
		);

		RWin32::ThrowIfFailed(compSch1.As(&compSch3));//转换到高版本交换链，用为接下来要调用高版本接口函数

		UpdateFrameIndex();
	}

	void D12SwapChain::NoAltEnter(IDXGIFactory5* factory, HWND hWnd)
	{
		//注意此行代码必须要放在CreateSwapChain后调用才有效，否则还是能用ALTENTER全屏
		RWin32::ThrowIfFailed(factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER), L"MakeWindowAssociation 成功\n", L"MakeWindowAssociation 失败\n");
	}

	void D12SwapChain::Resize(UINT newWidth, UINT newHeight)
	{
		DXGI_SWAP_CHAIN_DESC1 desc = GetDesc();

		RWin32::ThrowIfFailed(compSch3->ResizeBuffers(
			desc.BufferCount,
			newWidth, newHeight,
			desc.Format,
			desc.Flags
		));

		UpdateFrameIndex();//重新设置当前后缓冲下表
	}

	void D12SwapChain::UpdateFrameIndex()
	{
		frameIndex = compSch3->GetCurrentBackBufferIndex();
	}

	Microsoft::WRL::ComPtr<IDXGISwapChain3> D12SwapChain::GetSch()
	{
		return compSch3;
	}

	DXGI_SWAP_CHAIN_DESC1 D12SwapChain::GetDesc()
	{
		DXGI_SWAP_CHAIN_DESC1 desc;
		compSch3->GetDesc1(&desc);
		return desc;
	}

	UINT D12SwapChain::GetFrameIdx()
	{
		return frameIndex;
	}
}