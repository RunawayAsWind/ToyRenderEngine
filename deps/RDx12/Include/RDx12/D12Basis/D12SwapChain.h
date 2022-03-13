#pragma once
#include <dxgi1_6.h>
#include <d3d12.h>
#include <wrl.h>

namespace RDx12
{
	class D12SwapChain
	{
	private:
		Microsoft::WRL::ComPtr<IDXGISwapChain1> compSch1;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> compSch3;
		UINT frameIndex = 0;

	public:

		//只有老版本的交换链格式可以直接再这里设置MSAA，参见DX12龙书192页
		//老版本：DXGI_SWAP_CHAIN_DESC 新版本：DXGI_SWAP_CHAIN_DESC1
		//老版本：IDXGISwapChain 新版本：IDXGISwapChain1 IDXGISwapChain3
		//因此这里如果设置为true会报错 Flip model swapchains (DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL and DXGI_SWAP_EFFECT_FLIP_DISCARD) do not support multisampling.
		//新版本MSAA的解决方案https://www.gamedev.net/forums/topic/686804-d3d12-enabling-multi-sampling/
		DXGI_SWAP_CHAIN_DESC1 DefaultDesc(UINT backCount, DXGI_FORMAT fmt, UINT sampleCount, ID3D12Device4* dev);
		void CreateSwapChain(DXGI_SWAP_CHAIN_DESC1 desc,IDXGIFactory5* factory, ID3D12CommandQueue* pCmdQue, HWND hWnd);
		
		// 关闭ALT+ENTER键切换全屏的功能，在我们没有实现OnSize处理的时候记得关闭
		//https://docs.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgifactory-makewindowassociation
		void NoAltEnter(IDXGIFactory5* factory, HWND hWnd);
		
		void Resize(UINT newWidth, UINT newHeight);
		void UpdateFrameIndex();

		Microsoft::WRL::ComPtr<IDXGISwapChain3> GetSch();
		DXGI_SWAP_CHAIN_DESC1 GetDesc();
		UINT GetFrameIdx();

	};

}