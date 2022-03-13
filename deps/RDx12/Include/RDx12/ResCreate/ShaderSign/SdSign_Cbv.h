#pragma once
#include <RTmpl/Hash.h>
#include <RTmpl/Container/BlockManager.h>
#include "../DescHeap/HeapManager.h"
#include "../Res/Buffer/Res_DefaultSingleTypeBuffer.h"

namespace RDx12
{
	template<typename DataType>
	class ConstantBuffer
	{
	private:
		UINT rgIdx;									//Shader 中的 register 标号 如 register(b3) register(t3) 标号都为3
		UINT spIdx;									//Shader 中的 register 的space标号 如register(b4,space5) rg标号为4 space标号为5
		UINT maxNum;
		std::vector<std::shared_ptr<RDx12::Res_UploadSingleTypeBuffer>> vecFrameRes;
	public:
		ConstantBuffer(
			UINT rgIdx, UINT spIdx,
			UINT maxNum, UINT frameNum,bool isConstantBuffer
		) :rgIdx(rgIdx), spIdx(spIdx), maxNum(maxNum)
		{
			/*根据帧数创建帧资源*/
			for (int i = 0; i < frameNum; i++)
			{
				vecFrameRes.push_back(std::make_shared<Res_UploadSingleTypeBuffer>(sizeof(DataType), isConstantBuffer));
			}
		}

		/*资源初始化*/
		void InitRes(ID3D12Device4* dev)
		{
			for (auto& res : vecFrameRes)
			{
				res->CreateByMaxNum(maxNum, dev);
				res->SetMap(0, nullptr);
			}
		}

		/*获取帧资源*/
		std::shared_ptr<Res_UploadSingleTypeBuffer> GetFrameRes(int frameIdx)
		{
			return vecFrameRes[frameIdx];
		}

		constexpr UINT MaxNum() const noexcept { return maxNum; }
		constexpr UINT RgIdx() const noexcept { return rgIdx; }
		constexpr UINT SpIdx() const noexcept { return spIdx; }
	};


	template<typename DataType>
	class FrameResAlloc_RootCb
	{
	private:
		std::shared_ptr<RTmpl::BlockAllocation> elemAlloc;
		ConstantBuffer<DataType>* father;
		int rootCbvRpIdx = -1;
		int ElemIdx(){ return elemAlloc->range.first; }
		template <typename T> friend class Sign_RootCb;
	public:
		DataType data;

		bool Empty()
		{
			return elemAlloc == nullptr;
		}

		void Update(int frameIdx)
		{
			auto frameRes = father->GetFrameRes(frameIdx);//获取当前帧资源
			frameRes->CopyData(ElemIdx(), &data);//赋值到相应位置
		}

		/*数据设置到根常量*/
		void SetGraphicsRootConstantBufferView(int frameIdx, ID3D12GraphicsCommandList* cl)
		{
			auto frameRes = father->GetFrameRes(frameIdx);
			cl->SetGraphicsRootConstantBufferView(rootCbvRpIdx, frameRes->aGpu(ElemIdx()));
		}

		/*数据设置到根常量*/
		void SetComputeRootConstantBufferView(int frameIdx, ID3D12GraphicsCommandList* cl)
		{
			auto frameRes = father->GetFrameRes(frameIdx);
			cl->SetComputeRootConstantBufferView(rootCbvRpIdx, frameRes->aGpu(ElemIdx()));
		}
	};

	template<typename DataType>
	class Sign_RootCb
	{
	private:
		ConstantBuffer<DataType> cbuff;
		RTmpl::BlockManager blockManager;//根常量缓冲每次只绑定帧资源中的一部分，所以需要分发数据
		UINT rpIdx;

	public:
		Sign_RootCb(
			UINT rgIdx, UINT spIdx,
			UINT maxNum,UINT frameNum
		) :cbuff(rgIdx, spIdx, maxNum,frameNum,true), blockManager(RTmpl::Type_SimpleName<DataType>(), maxNum)
		{}

		/*
			初始化帧资源和根常量
			SetAsRootCbv -> SetGraphicsRootConstantBufferView
		*/
		void InitRootParam(std::vector<CD3DX12_ROOT_PARAMETER>& vec, ID3D12Device4* dev) {
			cbuff.InitRes(dev);
			vec.emplace_back(CD3DX12_ROOT_PARAMETER());
			vec.back().InitAsConstantBufferView(cbuff.RgIdx(), cbuff.SpIdx());
			rpIdx = vec.size() - 1;
		}

		/*获取单个数据*/
		FrameResAlloc_RootCb<DataType> Get(){
			FrameResAlloc_RootCb<DataType> subRes;
			subRes.elemAlloc = blockManager.Get(1);
			subRes.father = &cbuff;
			subRes.rootCbvRpIdx = rpIdx;
			return subRes;
		}

		constexpr UINT RpIdx() const noexcept { return rpIdx; }
	};






	template<typename DataType>
	class Sign_StructuredBuffer
	{
	private:
		ConstantBuffer<DataType> cbuff;
		UINT rpIdx;
	public:
		std::vector<DataType> vecData;//结构化缓冲区直接绑定整个帧资源，不需要分发，直接储存数据数组

		Sign_StructuredBuffer(
			UINT rgIdx, UINT spIdx,
			UINT maxNum, UINT frameNum
		) :cbuff(rgIdx, spIdx, maxNum, frameNum,false)
		{
		}

		/*
			初始化帧资源和根常量
			SetAsRootSrv -> SetGraphicsRootShaderResourceView
		*/
		void InitRootParam(std::vector<CD3DX12_ROOT_PARAMETER>& vec, ID3D12Device4* dev) {
			cbuff.InitRes(dev);
			vec.emplace_back(CD3DX12_ROOT_PARAMETER());
			vec.back().InitAsShaderResourceView(cbuff.RgIdx(), cbuff.SpIdx());
			rpIdx = vec.size() - 1;
		}

		void Update(int frameIdx)
		{
			auto frameRes = cbuff.GetFrameRes(frameIdx);
			for (int i = 0; i <vecData.size(); i++)
			{
				if (i < cbuff.MaxNum())
				{
					frameRes->CopyData(i, &vecData[i]);
				}
			}
		}

		void SetGraphicsRootShaderResourceView(int frameIdx, ID3D12GraphicsCommandList* cl)
		{
			auto frameRes = cbuff.GetFrameRes(frameIdx);
			cl->SetGraphicsRootShaderResourceView(rpIdx, frameRes->aGpu(0));
		}
	};






	template<typename DataType>
	class FrameResAlloc_Range
	{
	private:
		std::shared_ptr<RTmpl::BlockAllocation> elemAlloc;
		ConstantBuffer<DataType>* father;
		int ElemStartIdx(){ return elemAlloc->range.first; }
		template <typename T> friend class DesRange_Cbv;
	public:
		std::vector<DataType> vecData;

		void Update(int frameIdx)
		{
			auto frameRes = father->GetFrameRes(frameIdx);
			for (int i = 0; i < vecData.size(); i++)
			{
				frameRes->CopyData(i + ElemStartIdx(), &vecData[i]);
			}
		}

		void Bind(int frameIdx, D3D12_CPU_DESCRIPTOR_HANDLE* hCpu, ID3D12Device4* dev)
		{
			auto frameRes = father->GetFrameRes(frameIdx);
			for (int i = 0; i < vecData.size(); i++)
			{
				dev->CreateConstantBufferView(&frameRes->CbViewDesc(i + ElemStartIdx()), *(hCpu + i));
			}
		}
	};

	template<typename DataType>
	class DesRange_Cbv
	{
	private:
		ConstantBuffer<DataType> cbuff;
		RTmpl::BlockManager blockManager;
	public:
		CD3DX12_DESCRIPTOR_RANGE desRange;			//RootSign设置为DescriptorTable时需要的数据结构,必须要是持续化数据

		DesRange_Cbv(
			UINT rgIdx, UINT spIdx, UINT desNum,
			UINT rangeNum, UINT frameNum
		) :cbuff(rgIdx, spIdx, desNum*rangeNum, frameNum,true),
			blockManager(RTmpl::Type_SimpleName<DataType>(), desNum* rangeNum)
		{
			desRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, desNum, cbuff.RgIdx(), cbuff.SpIdx(), D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
		}

		void InitRes(ID3D12Device4* dev) {
			cbuff.InitRes(dev);
		}

		FrameResAlloc_Range<DataType> Get() {
			FrameResAlloc_Range<DataType> subRes;
			subRes.vecData.resize(DesNum());
			subRes.elemAlloc = blockManager.Get(DesNum());
			subRes.father = &cbuff;
			return subRes;
		}
		constexpr UINT DesNum() const noexcept { return desRange.NumDescriptors; }
	};






	class DesRange_Srv
	{
	public:
		CD3DX12_DESCRIPTOR_RANGE desRange;			//RootSign设置为DescriptorTable时需要的数据结构,必须要是持续化数据

		DesRange_Srv(UINT rgIdx, UINT spIdx, UINT desNum)
		{
			desRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, desNum, rgIdx, spIdx, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
		}

		constexpr UINT DesNum() const noexcept { return desRange.NumDescriptors; }
	};

	class DesRange_Uav
	{
	public:
		CD3DX12_DESCRIPTOR_RANGE desRange;

		DesRange_Uav(UINT rgIdx, UINT spIdx, UINT desNum)
		{
			desRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, desNum, rgIdx, spIdx, D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);
		}

		constexpr UINT DesNum() const noexcept { return desRange.NumDescriptors; }
	};


	class Sign_RootTable
	{
	protected:
		int frameNum;
		std::vector<CD3DX12_DESCRIPTOR_RANGE> vecRange;
		UINT rpIdx;
	public:
		Sign_RootTable(int frameNum) :frameNum(frameNum){}

		/*SetAsTable->Bind-> SetGraphicsRootDescriptorTable*/
		void InitRootParam(std::vector<CD3DX12_ROOT_PARAMETER>& vecRootParam)
		{
			vecRootParam.emplace_back(CD3DX12_ROOT_PARAMETER());
			vecRootParam.back().InitAsDescriptorTable(vecRange.size(), vecRange.data());
			rpIdx = vecRootParam.size() - 1;
		}
		constexpr UINT RpIdx() const noexcept { return rpIdx; }
	};

	struct SrvTexSet
	{
		std::string name;
		RDx12::DesRange_Srv range;
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> vechCpu;
	};
}