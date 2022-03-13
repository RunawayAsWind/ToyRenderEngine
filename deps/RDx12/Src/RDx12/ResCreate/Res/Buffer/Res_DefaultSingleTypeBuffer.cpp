#include "../Include/RDx12/ResCreate/Res/Buffer/Res_DefaultSingleTypeBuffer.h"

namespace RDx12
{
	void Res_DefaultSingleTypeBuffer::CreateByMaxNum(UINT maxNum, ID3D12Device4* dev)
	{
		bufferSize = maxNum * bufferStride;

		RWin32::ThrowIfFailed(dev->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&res)));

		uploadRes.CreateByMaxNum(maxNum, dev);
	}

	void Res_DefaultSingleTypeBuffer::CopyData(int eleidx, void* data)
	{
		uploadRes.CopyData(eleidx, data);
	}

	void Res_DefaultSingleTypeBuffer::SetMap(UINT subResIndex, D3D12_RANGE* readRange)
	{
		uploadRes.SetMap(subResIndex, readRange);
	}

	void Res_DefaultSingleTypeBuffer::Unmap(UINT subResIndex, D3D12_RANGE* readRange)
	{
		uploadRes.Unmap(subResIndex, readRange);
	}

	void Res_DefaultSingleTypeBuffer::UpdateRes(ID3D12GraphicsCommandList* cmdList)
	{
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(res.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		cmdList->CopyResource(res.Get(), uploadRes.GetRes());

		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(res.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
	};

	std::shared_ptr<Res_SingleTypeBuffer> GenSingleTypeBuffer(UINT typeSize, bool isConstantBuuffer, BufferType resType)
	{
		if (resType == BufferType::Default)
		{
			return std::make_shared<Res_DefaultSingleTypeBuffer>(typeSize, isConstantBuuffer);
		}
		else if (resType == BufferType::Upload)
		{
			return std::make_shared<Res_UploadSingleTypeBuffer>(typeSize, isConstantBuuffer);
		}
	}
}