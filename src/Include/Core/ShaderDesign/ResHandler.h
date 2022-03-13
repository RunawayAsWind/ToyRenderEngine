#pragma once
#include <RDx12/Geomertry/StructData.h>
#include <RDx12/ResCreate/Res/Texture/Texture.h>
#include <RDx12/ResCreate/DescHeap/GpuHandler.h>
#include "ShaderManager.h"
#include "../EcsCmpt/Cmpt_Trans.h"
#include "../EcsCmpt/Cmpt_Camera.h"
#include <RDx12\D12Basis\D12CmdQue.h>

class PipeLine_SkyBox
{
public:
	const char* Name() const noexcept { return mName.c_str(); }
	
	void InitorResize(
		std::string name,UINT size,UINT maxMip,
		DXGI_FORMAT diffuseFmt,DXGI_FORMAT preFilteredFmt,
		D3D12_CPU_DESCRIPTOR_HANDLE gCubeMap,D3D12_CPU_DESCRIPTOR_HANDLE gBrdfLUTMap,
		RDx12::D12CmdQue& que,ShaderManager& sdMg, RDx12::HeapManager& hpManager);

	void SetGpuData(ID3D12GraphicsCommandList* cl) { mGpuHandler.SetGpuData(0, cl); }

private:
	static const int camNum = 6;
	RDx12::GpuHandler mGpuHandler;
	std::string mName;
	UINT mSize;
	UINT mMaxMip;

	RDx12::RtvTex_Cube mResult_IBL_Diffuse;
	RDx12::MipTex_Cube mResult_IBL_PreFitered;
	
	std::vector<RDx12::RtvTex_Cube> mResult_IBL_PreFitered_Mip;
	std::vector<RDx12::DsvTex> mDsv_Mip;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPso_Diffuse;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> mPso_PreFiltered;

    Cmpt_Trans mCamTrans[camNum];
    Cmpt_Camera mCam[camNum];
	std::vector<std::vector<PassCb>> mPassCb{camNum};
    RDx12::Res_UploadSingleTypeBuffer mBufferPassCb = { sizeof(PassCb),true };

    Cmpt_Trans mObjTrans;
    ObjCb mObjCb;
    RDx12::Res_UploadSingleTypeBuffer mBufferObjCb = { sizeof(ObjCb),true };

    RDx12::Struct_MeshRes<RDx12::VertexData> mBufferMesh = { RDx12::BufferType::Default };

	void InitMeshAndTexRes(
		DXGI_FORMAT diffuseFmt,DXGI_FORMAT preFilteredFmt,
		ID3D12GraphicsCommandList* cl,RDx12::HeapManager& hpManager);

	void InitMatRes(
		DXGI_FORMAT diffuseFmt, DXGI_FORMAT preFilteredFmt,
		ShaderManager& sdMg, ID3D12Device4* dev);
	
	void RenderCubeRtv(
		UINT mipLevel,RDx12::RtvTex_Cube& rtvCube,
		ID3D12PipelineState* pso,
		ID3D12GraphicsCommandList* cl,ShaderManager& sdMg,  RDx12::HeapManager& hpManager);
};


class PipeLine_GBuffer
{
public:
	void InitorResize(
		UINT width, UINT height,
		std::vector<DXGI_FORMAT> fmtVec,//Gbuffer的各个通道格式
		DXGI_FORMAT dsvFmt,
		ShaderManager& sdMg, RDx12::HeapManager& hpManager);
	void RenderReady(ID3D12GraphicsCommandList* cl);
	void SetRenderTarget(ID3D12GraphicsCommandList* cl,UINT renderNum,UINT startIdx,D3D12_CPU_DESCRIPTOR_HANDLE *dsvhCpu);
	ID3D12PipelineState* GetPso();
	ID3D12PipelineState* GetSkinPso();
	void RenderEnd(ID3D12GraphicsCommandList* cl);
	void SetGpuData(ID3D12GraphicsCommandList* cl) { mGpuHandler.SetGpuData(0, cl); }
	UINT GetGbufferNum() const noexcept {return mVecRtv.size();}
	
private:
	RDx12::GpuHandler mGpuHandler;
    UINT mWidth;
    UINT mHeight;
	std::vector<RDx12::RtvTex> mVecRtv;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPso_Gbuffer;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPso_Gbuffer_Skin;
};

class PipeLine_SSAO
{
public:
	void InitorResize(
		UINT width,UINT height,
		DXGI_FORMAT fmt,
		ShaderManager& sdMg,RDx12::HeapManager& hpManager);
	void Render(ID3D12GraphicsCommandList* cl);
	void SetGpuData(ID3D12GraphicsCommandList* cl) { mGpuHandler.SetGpuData(0, cl); }
	ID3D12Resource* GetRes() { return mRtv.res.Get(); }
private:
	RDx12::GpuHandler mGpuHandler;
	UINT mWidth;
	UINT mHeight;
	RDx12::RtvTex mRtv;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPso_SSAO;
};

class PipeLine_RectBlur
{
public:
	PipeLine_RectBlur(RDx12::FrVec& frVec, ShaderManager& sdMg, RDx12::HeapManager& hpManager);
	void InitorResize(UINT width, UINT height, DXGI_FORMAT fmt);
	void SetBlurCore(float sigma);
	void Update();
	void Render(
		ID3D12Resource* input, D3D12_RESOURCE_STATES inputState,
		int blurCount, ID3D12GraphicsCommandList* cl);
	void SetGpuData(ID3D12GraphicsCommandList* cl);
	
private:
	UINT mWidth;
	UINT mHeight;
	RDx12::ComputeTex mComMap1;
	RDx12::ComputeTex mComMap2;

	int mFrameDirty = 0;
	RDx12::FrameResAlloc_Range<BlurCoreCb> mBlurCoreCb;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPso_HorzBlur;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPso_VertBlur;
	std::shared_ptr<RDx12::HeapAllocation> mHpAlloc_CsuGpu_Map1to2;
	std::shared_ptr<RDx12::HeapAllocation> mHpAlloc_CsuGpu_Map2to1;

	RDx12::FrVec* mFrVec;
	ShaderManager* mSdMg;
	RDx12::HeapManager* mHpManager;
};

class PipeLine_Sobel
{
public:
	PipeLine_Sobel( ShaderManager& sdMg, RDx12::HeapManager& hpManager):mSdMg(&sdMg), mHpManager(&hpManager)
	{
		mPso_Sobel = sdMg.GetSobelPso(hpManager.GetDev());
		if (mHpAlloc_CsuGpu_Map1to2 == nullptr) {
			mHpAlloc_CsuGpu_Map1to2 = hpManager.csuHeap_Gpu.Get(2);
		}
		if (mHpAlloc_CsuGpu_MapOut == nullptr) {
			mHpAlloc_CsuGpu_MapOut = hpManager.csuHeap_Gpu.Get(1);
		}
	}
	void InitorResize(UINT width, UINT height, DXGI_FORMAT fmt)
	{
		if (mWidth != width || mHeight != height)
		{
			mWidth = width;
			mHeight = height;
			mComMapOut.Create(width, height, fmt, *mHpManager);

			mHpManager->GetDev()->CopyDescriptorsSimple(1, mHpAlloc_CsuGpu_Map1to2->hCpu[1], mComMapOut.hCpu_Uav(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			mHpManager->GetDev()->CopyDescriptorsSimple(1, mHpAlloc_CsuGpu_MapOut->hCpu[0], mComMapOut.hCpu_Srv(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
	}
	void Render(
		ID3D12Resource* input, D3D12_RESOURCE_STATES inputState,
		D3D12_CPU_DESCRIPTOR_HANDLE inputSrvhCpu,
		ID3D12GraphicsCommandList* cl)
	{
		mHpManager->GetDev()->CopyDescriptorsSimple(1, mHpAlloc_CsuGpu_Map1to2->hCpu[0], inputSrvhCpu, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		CD3DX12_VIEWPORT viewPort = CD3DX12_VIEWPORT{ 0.0f, 0.0f, (float)mWidth, (float)mHeight, 0.0f, 1.0f };
		CD3DX12_RECT scissorRect = CD3DX12_RECT{ 0, 0, (int)mWidth, (int)mHeight };
		cl->RSSetViewports(1, &viewPort);
		cl->RSSetScissorRects(1, &scissorRect);

		//cl->SetComputeRootSignature(mSdMg->rootSign.Get());
		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(input, inputState, D3D12_RESOURCE_STATE_COMMON));
		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mComMapOut.res.Get(),D3D12_RESOURCE_STATE_COMMON,D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
		
		cl->SetPipelineState(mPso_Sobel.Get());
		cl->SetComputeRootDescriptorTable(mSdMg->sd_SobelTable.RpIdx(), mHpAlloc_CsuGpu_Map1to2->hGpu[0]);
		UINT numGroupsX = (UINT)ceilf(mWidth / 16.0f);
		UINT numGroupsY = (UINT)ceilf(mHeight / 16.0f);
		cl->Dispatch(numGroupsX, numGroupsY, 1);
		
		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mComMapOut.res.Get(),D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON));
		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(input, D3D12_RESOURCE_STATE_COMMON, inputState));

		//cl->SetGraphicsRootSignature(mSdMg->rootSign.Get());
	}
	void SetGpuData(ID3D12GraphicsCommandList* cl)
	{
		cl->SetGraphicsRootDescriptorTable(mSdMg->sd_SobelTable.RpIdx(), mHpAlloc_CsuGpu_MapOut->hGpu[0]);
	}

private:
	UINT mWidth;
	UINT mHeight;
	RDx12::ComputeTex mComMapOut;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPso_Sobel;
	std::shared_ptr<RDx12::HeapAllocation> mHpAlloc_CsuGpu_Map1to2;
	std::shared_ptr<RDx12::HeapAllocation> mHpAlloc_CsuGpu_MapOut;

	RDx12::FrVec* mFrVec;
	ShaderManager* mSdMg;
	RDx12::HeapManager* mHpManager;
};
