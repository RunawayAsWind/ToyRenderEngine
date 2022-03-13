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

		//ֻ���ϰ汾�Ľ�������ʽ����ֱ������������MSAA���μ�DX12����192ҳ
		//�ϰ汾��DXGI_SWAP_CHAIN_DESC �°汾��DXGI_SWAP_CHAIN_DESC1
		//�ϰ汾��IDXGISwapChain �°汾��IDXGISwapChain1 IDXGISwapChain3
		//��������������Ϊtrue�ᱨ�� Flip model swapchains (DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL and DXGI_SWAP_EFFECT_FLIP_DISCARD) do not support multisampling.
		//�°汾MSAA�Ľ������https://www.gamedev.net/forums/topic/686804-d3d12-enabling-multi-sampling/
		DXGI_SWAP_CHAIN_DESC1 DefaultDesc(UINT backCount, DXGI_FORMAT fmt, UINT sampleCount, ID3D12Device4* dev);
		void CreateSwapChain(DXGI_SWAP_CHAIN_DESC1 desc,IDXGIFactory5* factory, ID3D12CommandQueue* pCmdQue, HWND hWnd);
		
		// �ر�ALT+ENTER���л�ȫ���Ĺ��ܣ�������û��ʵ��OnSize�����ʱ��ǵùر�
		//https://docs.microsoft.com/en-us/windows/win32/api/dxgi/nf-dxgi-idxgifactory-makewindowassociation
		void NoAltEnter(IDXGIFactory5* factory, HWND hWnd);
		
		void Resize(UINT newWidth, UINT newHeight);
		void UpdateFrameIndex();

		Microsoft::WRL::ComPtr<IDXGISwapChain3> GetSch();
		DXGI_SWAP_CHAIN_DESC1 GetDesc();
		UINT GetFrameIdx();

	};

}