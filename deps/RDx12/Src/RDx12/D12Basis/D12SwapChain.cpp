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
		desc.Format = fmt;											//��㷺�����ݸ�ʽ�����ѡȡ��ʽ https://stackoverflow.com/questions/45636299/what-factors-determine-dxgi-format
		desc.SampleDesc.Count = msQualityLevels.NumQualityLevels > 0? msQualityLevels.SampleCount : 1;	//���ö����ģʽ����ʵֻ������1��ʵ��MSAA�ȿ���ݹ��ܻ���Ҫ����Ⱦ��RTT�ϣ�https://stackoverflow.com/questions/40110699/creating-a-swap-chain-with-msaa-fails
		desc.SampleDesc.Quality = msQualityLevels.NumQualityLevels > 0? msQualityLevels.NumQualityLevels-1 : 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;			//һ������½�����ֻ��Ҫ������������ˣ�SHADERINPUT������RTT
		desc.BufferCount = backCount;								//�󻺳�����
		desc.Scaling = DXGI_SCALING_NONE;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;			//���������һ��UWP����FLIP DISCARD https://docs.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_swap_effect  https://www.cnblogs.com/lc-cnblong/p/3365031.html https://docs.microsoft.com/zh-cn/windows/win32/direct3ddxgi/dxgi-flip-model
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
				nullptr,				//��ϵ���������Ƿ�ȫ��
				nullptr,				//��ϵ���������������
				&compSch1
			),
			L"�����������ɹ�\n",
			L"����������ʧ�ܣ�������\n"
		);

		RWin32::ThrowIfFailed(compSch1.As(&compSch3));//ת�����߰汾����������Ϊ������Ҫ���ø߰汾�ӿں���

		UpdateFrameIndex();
	}

	void D12SwapChain::NoAltEnter(IDXGIFactory5* factory, HWND hWnd)
	{
		//ע����д������Ҫ����CreateSwapChain����ò���Ч������������ALTENTERȫ��
		RWin32::ThrowIfFailed(factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER), L"MakeWindowAssociation �ɹ�\n", L"MakeWindowAssociation ʧ��\n");
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

		UpdateFrameIndex();//�������õ�ǰ�󻺳��±�
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