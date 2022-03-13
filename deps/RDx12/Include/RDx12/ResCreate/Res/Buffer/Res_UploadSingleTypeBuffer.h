#pragma once
#include "Res_SingleTypeBuffer.h"

namespace RDx12
{
	/*
		单类型的UploadBufferRes，数据类型为Buffer（非纹理）
		一般用于上传到GPU后可以每帧更改的buffer
		每帧变化的顶点（如CPU位移顶点实现海浪）
		或ObjectConstant，或 PassConstant 等Shader中用到的常量缓冲可以用此类型
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