#include <assert.h>
#include <RWin32/Win32Debug.h>
#include "../Include/RDx12/ResCreate/DescHeap/DescHeapCl.h"

namespace RDx12
{
	void DescHeapCl::CreateAsRTV(UINT desnum, ID3D12Device4* dev)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = desnum;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 0;

		Create(desc, dev);
	}

	void DescHeapCl::CreateAsDSV(UINT desnum, ID3D12Device4* dev)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		desc.NumDescriptors = desnum;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 0;

		Create(desc, dev);
	}

	void DescHeapCl::CreateAsCSU(UINT desnum, D3D12_DESCRIPTOR_HEAP_FLAGS flag, ID3D12Device4* dev)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		desc.NumDescriptors = desnum;
		desc.Flags = flag;
		desc.NodeMask = 0;

		Create(desc, dev);
	}

	void DescHeapCl::CreateAsSample(UINT desnum, ID3D12Device4* dev)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		desc.NumDescriptors = desnum;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 0;

		Create(desc, dev);
	}

	void DescHeapCl::Create(D3D12_DESCRIPTOR_HEAP_DESC desc,ID3D12Device4* dev)
	{
		RWin32::ThrowIfFailed(dev->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&compHeapDesc)), NULL, L"ÃèÊö·û¶Ñ´´½¨Ê§°Ü");

		hCPUHeapStart = compHeapDesc->GetCPUDescriptorHandleForHeapStart();

		if (desc.Flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
		{
			hGPUHeapStart = compHeapDesc->GetGPUDescriptorHandleForHeapStart();
		}
		else
		{
			hGPUHeapStart.ptr = 0;
		}

		HandleIncrementSize = dev->GetDescriptorHandleIncrementSize(desc.Type);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DescHeapCl::hCPU(UINT index)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
		handle.ptr = MakeOffsetted(hCPUHeapStart.ptr, index);
		return handle;
	}
	D3D12_GPU_DESCRIPTOR_HANDLE DescHeapCl::hGPU(UINT index)
	{
		assert(compHeapDesc->GetDesc().Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		D3D12_GPU_DESCRIPTOR_HANDLE handle;
		handle.ptr = MakeOffsetted(hGPUHeapStart.ptr, index);
		return handle;
	}

	void DescHeapCl::SetByCmdList(ID3D12GraphicsCommandList* cl)
	{
		ID3D12DescriptorHeap* descriptorHeaps[] = { compHeapDesc.Get() };
		cl->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	}

	SIZE_T DescHeapCl::MakeOffsetted(SIZE_T ptr, UINT index)
	{
		SIZE_T offsetted;
		offsetted = ptr + index * HandleIncrementSize;
		return offsetted;
	}

	D3D12_DESCRIPTOR_HEAP_DESC DescHeapCl::GetDesc()
	{
		return compHeapDesc->GetDesc();
	}

	ID3D12DescriptorHeap* DescHeapCl::GetHeap()
	{
		return compHeapDesc.Get();
	}
}



