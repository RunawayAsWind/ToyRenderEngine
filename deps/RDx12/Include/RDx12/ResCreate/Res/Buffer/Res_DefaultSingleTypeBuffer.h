#pragma once
#include "Res_UploadSingleTypeBuffer.h"


namespace RDx12
{
	/*
		单类型的DefaultBufferRes，数据类型为Buffer（非纹理）
		一般用于上传到GPU后不能更改的buffer
		每帧不变的顶点，顶点索引数据经常使用此类型
	*/
	class Res_DefaultSingleTypeBuffer : public Res_SingleTypeBuffer
	{
	public:
		Res_DefaultSingleTypeBuffer(UINT typeSize, bool isConstantBuffer) : Res_SingleTypeBuffer(typeSize, isConstantBuffer), uploadRes(typeSize, isConstantBuffer) {}
		void CreateByMaxNum(UINT maxNum, ID3D12Device4* dev) override;
		void CopyData(int eleidx, void* data) override;
		void SetMap(UINT subResIndex = 0, D3D12_RANGE* readRange = nullptr) override;
		void Unmap(UINT subResIndex = 0, D3D12_RANGE* readRange = nullptr) override;
		void UpdateRes(ID3D12GraphicsCommandList* cmdList) override;
	private:
		Res_UploadSingleTypeBuffer uploadRes;
	};

	enum class BufferType
	{
		Default,
		Upload
	};

	std::shared_ptr<Res_SingleTypeBuffer> GenSingleTypeBuffer(UINT bufferStride, bool isConstantBuuffer, BufferType resType);
}