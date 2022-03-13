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
		UINT rgIdx;									//Shader �е� register ��� �� register(b3) register(t3) ��Ŷ�Ϊ3
		UINT spIdx;									//Shader �е� register ��space��� ��register(b4,space5) rg���Ϊ4 space���Ϊ5
		UINT maxNum;
		std::vector<std::shared_ptr<RDx12::Res_UploadSingleTypeBuffer>> vecFrameRes;
	public:
		ConstantBuffer(
			UINT rgIdx, UINT spIdx,
			UINT maxNum, UINT frameNum,bool isConstantBuffer
		) :rgIdx(rgIdx), spIdx(spIdx), maxNum(maxNum)
		{
			/*����֡������֡��Դ*/
			for (int i = 0; i < frameNum; i++)
			{
				vecFrameRes.push_back(std::make_shared<Res_UploadSingleTypeBuffer>(sizeof(DataType), isConstantBuffer));
			}
		}

		/*��Դ��ʼ��*/
		void InitRes(ID3D12Device4* dev)
		{
			for (auto& res : vecFrameRes)
			{
				res->CreateByMaxNum(maxNum, dev);
				res->SetMap(0, nullptr);
			}
		}

		/*��ȡ֡��Դ*/
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
			auto frameRes = father->GetFrameRes(frameIdx);//��ȡ��ǰ֡��Դ
			frameRes->CopyData(ElemIdx(), &data);//��ֵ����Ӧλ��
		}

		/*�������õ�������*/
		void SetGraphicsRootConstantBufferView(int frameIdx, ID3D12GraphicsCommandList* cl)
		{
			auto frameRes = father->GetFrameRes(frameIdx);
			cl->SetGraphicsRootConstantBufferView(rootCbvRpIdx, frameRes->aGpu(ElemIdx()));
		}

		/*�������õ�������*/
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
		RTmpl::BlockManager blockManager;//����������ÿ��ֻ��֡��Դ�е�һ���֣�������Ҫ�ַ�����
		UINT rpIdx;

	public:
		Sign_RootCb(
			UINT rgIdx, UINT spIdx,
			UINT maxNum,UINT frameNum
		) :cbuff(rgIdx, spIdx, maxNum,frameNum,true), blockManager(RTmpl::Type_SimpleName<DataType>(), maxNum)
		{}

		/*
			��ʼ��֡��Դ�͸�����
			SetAsRootCbv -> SetGraphicsRootConstantBufferView
		*/
		void InitRootParam(std::vector<CD3DX12_ROOT_PARAMETER>& vec, ID3D12Device4* dev) {
			cbuff.InitRes(dev);
			vec.emplace_back(CD3DX12_ROOT_PARAMETER());
			vec.back().InitAsConstantBufferView(cbuff.RgIdx(), cbuff.SpIdx());
			rpIdx = vec.size() - 1;
		}

		/*��ȡ��������*/
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
		std::vector<DataType> vecData;//�ṹ��������ֱ�Ӱ�����֡��Դ������Ҫ�ַ���ֱ�Ӵ�����������

		Sign_StructuredBuffer(
			UINT rgIdx, UINT spIdx,
			UINT maxNum, UINT frameNum
		) :cbuff(rgIdx, spIdx, maxNum, frameNum,false)
		{
		}

		/*
			��ʼ��֡��Դ�͸�����
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
		CD3DX12_DESCRIPTOR_RANGE desRange;			//RootSign����ΪDescriptorTableʱ��Ҫ�����ݽṹ,����Ҫ�ǳ���������

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
		CD3DX12_DESCRIPTOR_RANGE desRange;			//RootSign����ΪDescriptorTableʱ��Ҫ�����ݽṹ,����Ҫ�ǳ���������

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