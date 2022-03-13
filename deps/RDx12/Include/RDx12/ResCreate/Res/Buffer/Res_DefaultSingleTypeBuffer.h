#pragma once
#include "Res_UploadSingleTypeBuffer.h"


namespace RDx12
{
	/*
		�����͵�DefaultBufferRes����������ΪBuffer��������
		һ�������ϴ���GPU���ܸ��ĵ�buffer
		ÿ֡����Ķ��㣬�����������ݾ���ʹ�ô�����
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