#include "../../../Include/Core/ShaderDesign/ResHandler.h"

void PipeLine_SkyBox::InitorResize(
	std::string name,UINT size,UINT maxMip,
	DXGI_FORMAT diffuseFmt,DXGI_FORMAT preFilteredFmt,
	D3D12_CPU_DESCRIPTOR_HANDLE gCubeMap,D3D12_CPU_DESCRIPTOR_HANDLE gBrdfLUTMap,
	RDx12::D12CmdQue& que,ShaderManager& sdMg, RDx12::HeapManager& hpManager)
{
	mName = name;
	mSize = size;
	mMaxMip = maxMip;

	que.ListReset();
	InitMeshAndTexRes(diffuseFmt, preFilteredFmt, que.GetCmdList(), hpManager);
	InitMatRes(diffuseFmt, preFilteredFmt, sdMg, hpManager.GetDev());

	//Render PreFitered_Mip	
	mGpuHandler.Init(
		{
			gCubeMap,
			{},
			{},
			gBrdfLUTMap 
		},
		sdMg.sd_CubeMap.RpIdx(),
		hpManager
	);
	std::vector<ID3D12Resource*> cubeMips;
	for (int mipLevel = 0; mipLevel < mMaxMip; mipLevel++)
	{
		RenderCubeRtv(
			mipLevel,mResult_IBL_PreFitered_Mip[mipLevel],
			mPso_PreFiltered.Get(),
			que.GetCmdList(), sdMg, hpManager
		);
		

		cubeMips.push_back(mResult_IBL_PreFitered_Mip[mipLevel].Res());
	}

	mResult_IBL_PreFitered.Create(
		cubeMips,
		que.GetCmdList(),
		hpManager
	);
	que.CloseAndExecute();
	que.FlushCmdQue();

	//Render Diffuse
	mGpuHandler.Change(1, mResult_IBL_Diffuse.hCpu_Srv(), hpManager.GetDev());
	mGpuHandler.Change(2, mResult_IBL_PreFitered.hCpu(), hpManager.GetDev());

	que.ListReset();
	RenderCubeRtv(
		0,mResult_IBL_Diffuse,
		mPso_Diffuse.Get(),
		que.GetCmdList(), sdMg, hpManager
	);
	que.CloseAndExecute();
	que.FlushCmdQue();

	//mGpuHandler.Change(2, mResult_IBL_PreFitered_Mip[0].hCpu_Srv(), hpManager.GetDev());
}

void PipeLine_SkyBox::InitMeshAndTexRes(
	DXGI_FORMAT diffuseFmt,DXGI_FORMAT preFilteredFmt,
	ID3D12GraphicsCommandList* cl,RDx12::HeapManager& hpManager)
{
	/*MeshRes*/
	RDx12::PureMesh sphere = RDx12::GeoGen::GetSphere("sphere", 1.0f, 20, 20);
	mBufferMesh.CreateByMaxNum(sphere.vecVertex.size(), sphere.vecIndex.size(), hpManager.GetDev());
	mBufferMesh.AddMesh(sphere);
	mBufferMesh.UpdateRes(cl);

	/*TexRes*/
	mResult_IBL_Diffuse.Create(mSize, mSize, diffuseFmt, hpManager);
	mResult_IBL_PreFitered_Mip.resize(mMaxMip);
	mDsv_Mip.resize(mMaxMip);
	for (int mipLevel = 0; mipLevel < mMaxMip; mipLevel++)
	{
		mResult_IBL_PreFitered_Mip[mipLevel].Create(mSize >> mipLevel, mSize >> mipLevel, preFilteredFmt, hpManager);
		mDsv_Mip[mipLevel].Create(mSize >> mipLevel, mSize >> mipLevel, hpManager);
	}
}

void PipeLine_SkyBox::InitMatRes(
	DXGI_FORMAT diffuseFmt,DXGI_FORMAT preFilteredFmt,
	ShaderManager& sdMg, ID3D12Device4* dev)
{
	/*Pso*/
	mPso_Diffuse = sdMg.GetIBL_DiffusePso({ diffuseFmt }, mDsv_Mip[0].GetDsvFmt(), dev);
	mPso_PreFiltered = sdMg.GetIBL_PreFilteredPso({ preFilteredFmt }, mDsv_Mip[0].GetDsvFmt(), dev);

	/*
	PassCb:X:camIdx Y:mipLevel
	mBufferPassCb
	|       cam1         |         cam2       |...
	------------------------------------------------
	|mip0 mip1 mip2....  | mip0 mip1 mip2.... |...
	*/
	mPassCb.resize(camNum);
	for (int camIdx = 0; camIdx < camNum; camIdx++)
	{
		mPassCb[camIdx].resize(mMaxMip);
	}
	mBufferPassCb.CreateByMaxNum(camNum * mMaxMip, dev);
	mBufferPassCb.SetMap();

	std::vector<DirectX::XMFLOAT4> quatvec = RDx12::MathHelper::GetCubeCameraQuat(DirectX::XMFLOAT3(0, 0, 0), false);
	for (int camIdx = 0; camIdx < camNum; camIdx++)
	{
		mCamTrans[camIdx].mOffsetQuat = quatvec[camIdx];
		mCamTrans[camIdx].ComputeGobalTrans(nullptr);
		mCam[camIdx].SetFrustum(0.1f, 1000, 1, 90);

		Cmpt_Camera::Update()(&mCam[camIdx], &mCamTrans[camIdx]);

		for (int mipLevel = 0; mipLevel < mMaxMip; mipLevel++)
		{
			mPassCb[camIdx][mipLevel] = GetPassCb(
				mCamTrans[camIdx].mPos,
				DirectX::XMLoadFloat4x4(&mCam[camIdx].mViewMat),
				DirectX::XMLoadFloat4x4(&mCam[camIdx].mProjMat),
				DirectX::XMFLOAT2(mSize >> mipLevel, mSize >> mipLevel),
				mCam->mNearSize,
				mCam->mNearZ,
				mCam->mFarZ,
				0,
				0
			);
			mPassCb[camIdx][mipLevel].cbPerObjectPad1 = 0.05f * mipLevel;//用PassCb没有用到的占位符来上传计算PreFitered所需的粗造毒
			mBufferPassCb.CopyData(camIdx * mMaxMip + mipLevel, &mPassCb[camIdx][mipLevel]);
		}
	}

	/*ObjCbRes*/
	mBufferObjCb.CreateByMaxNum(1, dev);
	mBufferObjCb.SetMap();

	mObjCb = GetObjCb(
		DirectX::XMLoadFloat4x4(&mObjTrans.mWorldMat),
		DirectX::XMLoadFloat4x4(&mObjCb.gTexTransform),
		0, 0, 0, 0
	);
	mBufferObjCb.CopyData(0, &mObjCb);
}

void PipeLine_SkyBox::RenderCubeRtv(
	UINT mipLevel,RDx12::RtvTex_Cube& rtvCube,
	ID3D12PipelineState* pso,
	ID3D12GraphicsCommandList* cl, ShaderManager& sdMg, RDx12::HeapManager& hpManager)
{
	CD3DX12_VIEWPORT viewPort = CD3DX12_VIEWPORT{ 0.0f, 0.0f, (float)(mSize>> mipLevel), (float)(mSize >> mipLevel), 0.0f, 1.0f };
	CD3DX12_RECT scissorRect = CD3DX12_RECT{ 0, 0, (int)(mSize >> mipLevel), (int)(mSize >> mipLevel) };
	//开始记录命令
	cl->SetGraphicsRootSignature(sdMg.rootSign.Get());
	cl->RSSetViewports(1, &viewPort);
	cl->RSSetScissorRects(1, &scissorRect);

	// Change to RENDER_TARGET.
	cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rtvCube.res.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));

	cl->IASetVertexBuffers(0, 1, &(mBufferMesh.VertexBufferView()));
	cl->IASetIndexBuffer(&(mBufferMesh.IndexBufferView()));
	RDx12::SubMesh submesh = mBufferMesh.GetSubMesh(0);
	cl->IASetPrimitiveTopology(submesh.topology);

	cl->SetGraphicsRootConstantBufferView(sdMg.sd_ObjCb.RpIdx(), mBufferObjCb.aGpu(0));
	hpManager.csuHeap_Gpu.SetByCmdList(0, cl);
	mGpuHandler.SetGpuData(0,cl);
	cl->SetPipelineState(pso);

	for (int camIdx = 0; camIdx < camNum; camIdx++)
	{
		std::cout << camIdx << std::endl;
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		cl->ClearRenderTargetView(rtvCube.hCpu_Rtv(camIdx), clearColor, 0, nullptr);
		cl->ClearDepthStencilView(mDsv_Mip[mipLevel].hCpu_Dsv(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		cl->OMSetRenderTargets(1, &(rtvCube.hCpu_Rtv(camIdx)), true, &mDsv_Mip[mipLevel].hCpu_Dsv());

		cl->SetGraphicsRootConstantBufferView(sdMg.sd_PassCb.RpIdx(), mBufferPassCb.aGpu(camIdx * mMaxMip + mipLevel));
		cl->DrawIndexedInstanced(submesh.indexCount, 1, submesh.startIndexLocation, submesh.startVertexLocation, 0);
	}

	//又一个资源屏障，用于确定渲染已经结束可以提交画面去显示了
	cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rtvCube.res.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));

}

void PipeLine_GBuffer::InitorResize(
	UINT width,UINT height,
	std::vector<DXGI_FORMAT> fmtVec,
	DXGI_FORMAT dsvFmt,
	ShaderManager& sdMg,RDx12::HeapManager& hpManager)
{
	assert(fmtVec.size() > 0);

	mWidth = width;
	mHeight = height;

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> hCpuVec;
	mVecRtv.resize(fmtVec.size());
	//std::cout << "!!!!!!!!!!!!!!!! "<<mVecRtv.size() << std::endl;
	for (int i = 0; i < fmtVec.size(); i++)
	{
		mVecRtv[i].Create(width, height, fmtVec[i], hpManager, {0.0f,0.0f,0.0f,1.0f});
		hCpuVec.push_back(mVecRtv[i].hCpu_Srv());
	}
	mGpuHandler.Init(hCpuVec, sdMg.sd_Gbuffer.RpIdx(), hpManager);
	mPso_Gbuffer = sdMg.GetGbufferPso(fmtVec, dsvFmt, hpManager.GetDev());
	mPso_Gbuffer_Skin = sdMg.GetGbufferSkinPso(fmtVec, dsvFmt, hpManager.GetDev());
}

void PipeLine_GBuffer::RenderReady(ID3D12GraphicsCommandList* cl)
{
	CD3DX12_VIEWPORT viewPort = CD3DX12_VIEWPORT{ 0.0f, 0.0f, (float)mWidth, (float)mHeight, 0.0f, 1.0f };
	CD3DX12_RECT scissorRect = CD3DX12_RECT{ 0, 0, (int)mWidth, (int)mHeight };
	cl->RSSetViewports(1, &viewPort);
	cl->RSSetScissorRects(1, &scissorRect);

	for (int i = 0; i < mVecRtv.size(); i++)
	{
		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mVecRtv[i].res.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
		const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		cl->ClearRenderTargetView(mVecRtv[i].hCpu_Rtv(), clearColor, 0, nullptr);
	}
}


void PipeLine_GBuffer::SetRenderTarget(ID3D12GraphicsCommandList* cl, UINT renderNum, UINT startIdx, D3D12_CPU_DESCRIPTOR_HANDLE* dsvhCpu)
{
	cl->OMSetRenderTargets(renderNum, &mVecRtv[startIdx].hCpu_Rtv(), true, dsvhCpu);
}

ID3D12PipelineState* PipeLine_GBuffer::GetPso()
{
	return mPso_Gbuffer.Get();
}

ID3D12PipelineState* PipeLine_GBuffer::GetSkinPso()
{
	return mPso_Gbuffer_Skin.Get();
}

void PipeLine_GBuffer::RenderEnd(ID3D12GraphicsCommandList* cl)
{
	for (int i = 0; i < mVecRtv.size(); i++)
	{
		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mVecRtv[i].res.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
	}
}

void PipeLine_SSAO::InitorResize(
	UINT width, UINT height,
	DXGI_FORMAT fmt,
	ShaderManager& sdMg, RDx12::HeapManager& hpManager)
{
	mWidth = width;
	mHeight = height;
	mRtv.Create(width, height, fmt, hpManager, { 0.0f, 0.0f, 0.0f, 1.0f });
	mGpuHandler.Init({ mRtv.hCpu_Srv() }, sdMg.sd_SsaoMap.RpIdx(), hpManager);
	mPso_SSAO = sdMg.GetSsaoPso({ fmt }, hpManager.GetDev());
}

void PipeLine_SSAO::Render(ID3D12GraphicsCommandList* cl)
{
	CD3DX12_VIEWPORT viewPort = CD3DX12_VIEWPORT{ 0.0f, 0.0f, (float)mWidth, (float)mHeight, 0.0f, 1.0f };
	CD3DX12_RECT scissorRect = CD3DX12_RECT{ 0, 0, (int)mWidth, (int)mHeight };
	cl->RSSetViewports(1, &viewPort);
	cl->RSSetScissorRects(1, &scissorRect);

	cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRtv.res.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
	
	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	cl->ClearRenderTargetView(mRtv.hCpu_Rtv(), clearColor, 0, nullptr);
	cl->OMSetRenderTargets(1, &mRtv.hCpu_Rtv(), true, nullptr);
	
	cl->SetPipelineState(mPso_SSAO.Get());
	
	cl->IASetVertexBuffers(0, 0, nullptr);
	cl->IASetIndexBuffer(nullptr);
	cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cl->DrawInstanced(6, 1, 0, 0);
	
	cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mRtv.res.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ));
}

PipeLine_RectBlur::PipeLine_RectBlur(RDx12::FrVec& frVec, ShaderManager& sdMg, RDx12::HeapManager& hpManager) :mFrVec(&frVec), mSdMg(&sdMg), mHpManager(&hpManager)
{
	mFrameDirty = frVec.GetFrNum();
	mBlurCoreCb = sdMg.sd_BlurTable.Get();
	mPso_HorzBlur = sdMg.GetHorzBlurPso(hpManager.GetDev());
	mPso_VertBlur = sdMg.GetVertBlurPso(hpManager.GetDev());
	if (mHpAlloc_CsuGpu_Map1to2 == nullptr) {
		mHpAlloc_CsuGpu_Map1to2 = hpManager.csuHeap_Gpu.Get(3);
	}
	if (mHpAlloc_CsuGpu_Map2to1 == nullptr) {
		mHpAlloc_CsuGpu_Map2to1 = hpManager.csuHeap_Gpu.Get(3);
	}
}

void PipeLine_RectBlur::InitorResize(UINT width, UINT height, DXGI_FORMAT fmt)
{
	if (mWidth != width || mHeight != height)
	{
		mWidth = width;
		mHeight = height;
		mComMap1.Create(width, height, fmt, *mHpManager);
		mComMap2.Create(width, height, fmt, *mHpManager);

		mHpManager->GetDev()->CopyDescriptorsSimple(1, mHpAlloc_CsuGpu_Map1to2->hCpu[1], mComMap1.hCpu_Srv(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		mHpManager->GetDev()->CopyDescriptorsSimple(1, mHpAlloc_CsuGpu_Map1to2->hCpu[2], mComMap2.hCpu_Uav(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		mHpManager->GetDev()->CopyDescriptorsSimple(1, mHpAlloc_CsuGpu_Map2to1->hCpu[1], mComMap2.hCpu_Srv(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		mHpManager->GetDev()->CopyDescriptorsSimple(1, mHpAlloc_CsuGpu_Map2to1->hCpu[2], mComMap1.hCpu_Uav(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
}

void PipeLine_RectBlur::SetBlurCore(float sigma)
{
	// Estimate the blur radius based on sigma since sigma controls the "width" of the bell curve.
	// For example, for sigma = 3, the width of the bell curve is 
	int gBlurRadius = (int)ceil(2.0f * sigma);
	std::vector<float> gBlurWeight;

	float twoSigma2 = 2.0f * sigma * sigma;

	gBlurWeight.resize(2 * gBlurRadius + 1);

	float weightSum = 0.0f;

	for (int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		float x = (float)i;

		gBlurWeight[i + gBlurRadius] = expf(-x * x / twoSigma2);

		weightSum += gBlurWeight[i + gBlurRadius];
	}

	// Divide by the sum so all the weights add up to 1.0.
	for (int i = 0; i < gBlurWeight.size(); ++i)
	{
		gBlurWeight[i] /= weightSum;
	}


	mBlurCoreCb.vecData[0].gBlurRadius = gBlurRadius;
	for (int i = 0; i < gBlurWeight.size(); i++)
	{
		if (i < 11)
		{
			mBlurCoreCb.vecData[0].gBlurWeight[i] = gBlurWeight[i];
		}
	}
	mFrameDirty = mFrVec->GetFrNum();
}

void PipeLine_RectBlur::Update()
{
	if (mFrameDirty > 0)
	{
		mBlurCoreCb.Update(mFrVec->GetNowFrIdx());
		mFrameDirty--;
	}
	mBlurCoreCb.Bind(mFrVec->GetNowFrIdx(), &mHpAlloc_CsuGpu_Map1to2->hCpu[0], mHpManager->GetDev());
	mBlurCoreCb.Bind(mFrVec->GetNowFrIdx(), &mHpAlloc_CsuGpu_Map2to1->hCpu[0], mHpManager->GetDev());
}

void PipeLine_RectBlur::Render(
	ID3D12Resource* input, D3D12_RESOURCE_STATES inputState,
	int blurCount, ID3D12GraphicsCommandList* cl)
{
	CD3DX12_VIEWPORT viewPort = CD3DX12_VIEWPORT{ 0.0f, 0.0f, (float)mWidth, (float)mHeight, 0.0f, 1.0f };
	CD3DX12_RECT scissorRect = CD3DX12_RECT{ 0, 0, (int)mWidth, (int)mHeight };
	cl->RSSetViewports(1, &viewPort);
	cl->RSSetScissorRects(1, &scissorRect);

	// Copy the input (back-buffer in this example) to BlurMap0.
	cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(input,
		inputState, D3D12_RESOURCE_STATE_COPY_SOURCE));
	cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mComMap1.res.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
	cl->CopyResource(mComMap1.res.Get(), input);
	cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(input,
		D3D12_RESOURCE_STATE_COPY_SOURCE, inputState));

	//cl->SetComputeRootSignature(mSdMg->rootSign.Get());

	for (int i = 0; i < blurCount; ++i)
	{
		// Horizontal Blur pass.
		if (i == 0)
		{
			cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mComMap1.res.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));
			cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mComMap2.res.Get(),
				D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
		}
		else
		{
			cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mComMap1.res.Get(),
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));
			cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mComMap2.res.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
		}
		cl->SetPipelineState(mPso_HorzBlur.Get());
		cl->SetComputeRootDescriptorTable(mSdMg->sd_BlurTable.RpIdx(), mHpAlloc_CsuGpu_Map1to2->hGpu[0]);

		// How many groups do we need to dispatch to cover a row of pixels, where each
		// group covers 256 pixels (the 256 is defined in the ComputeShader).
		UINT numGroupsX = (UINT)ceilf(mWidth / 256.0f);
		cl->Dispatch(numGroupsX, mHeight, 1);


		// Vertical Blur pass.
		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mComMap1.res.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_UNORDERED_ACCESS));
		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mComMap2.res.Get(),
			D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_GENERIC_READ));
		
		cl->SetPipelineState(mPso_VertBlur.Get());
		cl->SetComputeRootDescriptorTable(mSdMg->sd_BlurTable.RpIdx(), mHpAlloc_CsuGpu_Map2to1->hGpu[0]);

		// How many groups do we need to dispatch to cover a column of pixels, where each
		// group covers 256 pixels  (the 256 is defined in the ComputeShader).
		UINT numGroupsY = (UINT)ceilf(mHeight / 256.0f);
		cl->Dispatch(mWidth, numGroupsY, 1);

		if (i == blurCount - 1)
		{
			cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mComMap1.res.Get(),
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON));
			cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mComMap2.res.Get(),
				D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COMMON));
		}
	}
	//cl->SetGraphicsRootSignature(mSdMg->rootSign.Get());
}

void PipeLine_RectBlur::SetGpuData(ID3D12GraphicsCommandList* cl)
{
	cl->SetGraphicsRootDescriptorTable(mSdMg->sd_BlurTable.RpIdx(), mHpAlloc_CsuGpu_Map1to2->hGpu[0]);
}