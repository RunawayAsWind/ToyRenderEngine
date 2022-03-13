#include "../Include/RDx12/ResCreate/Res/Buffer/Res_SingleTypeBuffer.h"

namespace RDx12 
{
	Res_SingleTypeBuffer::Res_SingleTypeBuffer(UINT typeSize, bool isConstantBuffer)
	{
		isCb = isConstantBuffer;
		if (isConstantBuffer == true)
			bufferStride = CalcConstantBufferByteSize(typeSize);
		else
			bufferStride = typeSize;
	}

	D3D12_GPU_VIRTUAL_ADDRESS Res_SingleTypeBuffer::aGpu(UINT elementIndex)
	{
		return res->GetGPUVirtualAddress() + elementIndex * bufferStride;
	}

	D3D12_INDEX_BUFFER_VIEW Res_SingleTypeBuffer::IndexBufferView()
	{
		D3D12_INDEX_BUFFER_VIEW ibv = {};
		ibv.BufferLocation = res->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = bufferSize;

		return ibv;
	}

	D3D12_VERTEX_BUFFER_VIEW Res_SingleTypeBuffer::VertexBufferView()
	{
		D3D12_VERTEX_BUFFER_VIEW vbv = {};
		vbv.BufferLocation = res->GetGPUVirtualAddress();
		vbv.StrideInBytes = bufferStride;
		vbv.SizeInBytes = bufferSize;

		return vbv;
	}

	D3D12_CONSTANT_BUFFER_VIEW_DESC Res_SingleTypeBuffer::CbViewDesc(UINT idx)
	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
		cbvDesc.BufferLocation = aGpu(idx);
		cbvDesc.SizeInBytes = bufferStride;
		return cbvDesc;
	}
}