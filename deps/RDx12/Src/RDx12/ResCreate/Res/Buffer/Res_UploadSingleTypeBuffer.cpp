#include "../Include/RDx12/ResCreate/Res/Buffer/Res_UploadSingleTypeBuffer.h"

namespace RDx12
{
	void Res_UploadSingleTypeBuffer::CreateByMaxNum(UINT maxNum, ID3D12Device4* dev)
	{
		bufferSize = maxNum * bufferStride;

		RWin32::ThrowIfFailed(dev->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&res)));
	}

	void Res_UploadSingleTypeBuffer::CopyData(int eleidx, void* data)
	{
		memcpy(aCpu(eleidx), data, bufferStride);
	}

	void Res_UploadSingleTypeBuffer::SetMap(UINT subResIndex, D3D12_RANGE* readRange)
	{
		RWin32::ThrowIfFailed(res->Map(subResIndex, readRange, reinterpret_cast<void**>(&cpuBuffer)));
	}

	void Res_UploadSingleTypeBuffer::Unmap(UINT subResIndex, D3D12_RANGE* readRange)
	{
		res->Unmap(subResIndex, readRange);
	}

	void* Res_UploadSingleTypeBuffer::aCpu(int elementIndex)
	{
		return &cpuBuffer[elementIndex * bufferStride];
	}
}