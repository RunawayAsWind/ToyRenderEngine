#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace RDx12
{
	//https://docs.microsoft.com/zh-cn/windows/win32/direct3d12/creating-descriptor-heaps 
	//最新实现：https://github.com/galek/SDL-Directx12/blob/master/helpers.h
	class DescHeapCl
	{
	public:
		void CreateAsRTV(UINT desnum, ID3D12Device4* pID3D12Device4);
		void CreateAsDSV(UINT desnum, ID3D12Device4* pID3D12Device4);
		void CreateAsCSU(UINT desnum, D3D12_DESCRIPTOR_HEAP_FLAGS flag, ID3D12Device4* pID3D12Device4);
		void CreateAsSample(UINT desnum, ID3D12Device4* pID3D12Device4);
		void Create(D3D12_DESCRIPTOR_HEAP_DESC desc,ID3D12Device4* pID3D12Device4);
		
		D3D12_CPU_DESCRIPTOR_HANDLE hCPU(UINT index);
		D3D12_GPU_DESCRIPTOR_HANDLE hGPU(UINT index);

		void SetByCmdList(ID3D12GraphicsCommandList* cl);
		D3D12_DESCRIPTOR_HEAP_DESC GetDesc();
		ID3D12DescriptorHeap* GetHeap();
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> compHeapDesc;
		D3D12_CPU_DESCRIPTOR_HANDLE hCPUHeapStart;
		D3D12_GPU_DESCRIPTOR_HANDLE hGPUHeapStart;
		UINT HandleIncrementSize;

		SIZE_T MakeOffsetted(SIZE_T ptr, UINT index);
	};
}