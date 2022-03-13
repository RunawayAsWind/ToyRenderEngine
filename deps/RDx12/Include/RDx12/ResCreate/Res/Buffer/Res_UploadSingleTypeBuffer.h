#pragma once
#include "Res_SingleTypeBuffer.h"

namespace RDx12
{
	/*
		�����͵�UploadBufferRes����������ΪBuffer��������
		һ�������ϴ���GPU�����ÿ֡���ĵ�buffer
		ÿ֡�仯�Ķ��㣨��CPUλ�ƶ���ʵ�ֺ��ˣ�
		��ObjectConstant���� PassConstant ��Shader���õ��ĳ�����������ô�����
	*/
	class Res_UploadSingleTypeBuffer : public Res_SingleTypeBuffer
	{
	public:
		Res_UploadSingleTypeBuffer(UINT typeSize, bool isConstantBuffer) :Res_SingleTypeBuffer(typeSize,isConstantBuffer) {}
		void CreateByMaxNum(UINT maxNum, ID3D12Device4* dev) override;
		void CopyData(int eleidx, void* data) override;
		void SetMap(UINT subResIndex = 0, D3D12_RANGE* readRange = nullptr) override;
		void Unmap(UINT subResIndex = 0, D3D12_RANGE* readRange = nullptr) override;
		void UpdateRes(ID3D12GraphicsCommandList* cmdList = nullptr) override{};
	private:
		BYTE* cpuBuffer = nullptr;
		void* aCpu(int elementIndex);
	};
}