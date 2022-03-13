#pragma once
#include "../../Include/FrameWork_Release/TestScene001_Base.h"
#include "../../Global/GobalPath.h"
#include <RWin32/FileHandler.h>
#include <wincodec.h>
#include <algorithm>
using namespace DirectX;


void TestScene001_Base::SetAsRenderItem(std::shared_ptr<SceneNode> nd, int meshIdx, RenderLayer layer, int objTexCbIdx)
{
	nd->Add<Cmpt_ObjCb, Cmpt_Mesh>();

	nd->Get<Cmpt_Mesh>()->mMeshIdx = meshIdx;
	
	Cmpt_ObjCb* objCb = nd->Get<Cmpt_ObjCb>();
	if (objCb->mObjCb.Empty())
	{
		objCb->AllocRes(sdMg.sd_ObjCb);
	}
	objCb->mObjCb.data.gRenderIdx = (int)layer;
	objCb->mObjCb.data.gObjTexCbIdx = (int)objTexCbIdx;

	layerDirty = true;
	meshDirty = true;
}

void TestScene001_Base::SetAsCamera(std::shared_ptr<SceneNode> nd, CameraLayer layerIdx)
{
	nd->Get<Cmpt_Trans>()->MustDirty();//强行dirty保证Camera更新viewMat
	nd->Add<Cmpt_Camera, Cmpt_PassCb>();

	Cmpt_PassCb* passCb = nd->Get<Cmpt_PassCb>();
	if (passCb->mPassCb.Empty())
	{
		passCb->AllocRes(sdMg.sd_PassCb);
	}

	nd->Get<Cmpt_Camera>()->SetFrustum(0.1f, 1000, static_cast<float>(baseSwch.GetDesc().Width) / baseSwch.GetDesc().Height, 45);
	layerDirty = true;
}

void TestScene001_Base::SetAsSkinRenderItem(std::shared_ptr<SceneNode> nd, int meshIdx, RenderLayer layer, int objTexCbIdx)
{
	nd->Add<Cmpt_ObjCb, Cmpt_SkinMesh>();

	Cmpt_SkinMesh* skin = nd->Get<Cmpt_SkinMesh>();
	skin->mSkinMeshIdx = meshIdx;
	if (skin->mSkinnedCb.Empty())
	{
		skin->AllocRes(sdMg.sd_SkinnedCb);
	}

	Cmpt_ObjCb* objCb = nd->Get<Cmpt_ObjCb>();
	if (objCb->mObjCb.Empty())
	{
		objCb->AllocRes(sdMg.sd_ObjCb);
	}
	objCb->mObjCb.data.gRenderIdx = (int)layer;
	objCb->mObjCb.data.gObjTexCbIdx = (int)objTexCbIdx;

	layerDirty = true;
	meshDirty = true;
}

void TestScene001_Base::SetAsSkinRoot(std::shared_ptr<SceneNode> nd, float endTime)
{
	nd->Add<Cmpt_SkinRoot>();
	nd->Get<Cmpt_SkinRoot>()->mClipVec[0].endTime = endTime;
}

void TestScene001_Base::DeleteFromLayer(std::shared_ptr<SceneNode> nd, std::vector<std::shared_ptr<SceneNode>>* vecLayer, int layerCount)
{
	for (int i = 0; i < layerCount; i++)
	{
		for (auto iter = vecLayer[i].begin(); iter != vecLayer[i].end(); )
		{
			if (*iter == nd)
				iter = vecLayer[i].erase(iter);
			else
				iter++;
		}
	}
}

void TestScene001_Base::DeleteFromAllLayer(std::shared_ptr<SceneNode> nd)
{
	DeleteFromLayer(nd, mVecRenderLayer, (int)RenderLayer::Count);
	DeleteFromLayer(nd, mVecCamLayer, (int)CameraLayer::Count);
}

int TestScene001_Base::CheckRepeat(std::vector<int>& vec, int num)
{
	auto iter = std::find(vec.begin(), vec.end(), num);
	if (iter == vec.end())
	{
		vec.push_back(num);
		return vec.size() - 1;
	}
	else
	{
		return std::distance(vec.begin(), iter);
	}
}

void TestScene001_Base::ClearAllNode() {
	auto delVec = Scene::GetNodeVec(sc.root, [](std::shared_ptr<SceneNode> nd) {return true; });
	for (auto delNd : delVec)
	{
		Scene::RemoveNode(delNd);
	}

	layerDirty = true;
	meshDirty = true;
};

void TestScene001_Base::UpdateLayer()
{
	if (layerDirty)
	{
		for (auto& vec : mVecRenderLayer)
		{
			vec.clear();
		}
		for (auto& vec : mVecCamLayer)
		{
			vec.clear();
		}
		UpdateLayerRcs(sc.root);
		layerDirty = false;
	}
}

void TestScene001_Base::UpdateLayerRcs(std::shared_ptr<SceneNode> nd)
{
	if (nd->IsRenderNode())
	{
		int gRenderIdx = nd->Get<Cmpt_ObjCb>()->mObjCb.data.gRenderIdx;
		mVecRenderLayer[gRenderIdx].push_back(nd);
		
		if (gRenderIdx != (int)RenderLayer::Sky)
		{
			if (nd->is_selected)
			{
				mVecRenderLayer[(int)RenderLayer::Select].push_back(nd);
			}
			else if (!nd->is_selected)
			{
				mVecRenderLayer[(int)RenderLayer::NotSelect].push_back(nd);
			}
		}
	}
	if (nd->IsCameraNode())
	{
		mVecCamLayer[(int)CameraLayer::Main].push_back(nd);
	}
	for (auto childNode : nd->child)
	{
		UpdateLayerRcs(childNode);
	}
}

void TestScene001_Base::UpdateMesh()
{
	if (meshDirty)
	{
		std::vector<int> vecMeshResIdx;
		std::vector<int> vecSkinMeshResIdx;
		UpdateMeshRcs(sc.root, vecMeshResIdx, vecSkinMeshResIdx);
		int vertexNum = 10; int indexNum = 10;
		for (auto meshIdx : vecMeshResIdx)
		{
			vertexNum += sc.vecPureMesh[meshIdx].vecVertex.size();
			indexNum += sc.vecPureMesh[meshIdx].vecIndex.size();
		}
		int skinVertexNum = 10; int skinIndexNum = 10;
		for (auto meshIdx : vecSkinMeshResIdx)
		{
			skinVertexNum += sc.vecSkinMesh[meshIdx].vecVertex.size();
			skinIndexNum += sc.vecSkinMesh[meshIdx].vecIndex.size();
		}
		this->vertexNum = vertexNum;
		this->indexNum = indexNum;
		this->skinVertexNum = skinVertexNum;
		this->skinIndexNum = skinIndexNum;

		dx12Base.baseCmdQue.WaitFenceToFeNum(dx12Base.baseCmdQue.GetMainFeNum());
		vecMeshRes.CreateByMaxNum(vertexNum, indexNum, dx12Base.GetDevice());
		vecSkinMeshRes.CreateByMaxNum(skinVertexNum, skinIndexNum, dx12Base.GetDevice());

		dx12Base.baseCmdQue.FlushCmdQue();
		dx12Base.baseCmdQue.ListReset();
		for (auto meshIdx : vecMeshResIdx)
		{
			vecMeshRes.AddMesh(sc.vecPureMesh[meshIdx]);
		}
		for (auto meshIdx : vecSkinMeshResIdx)
		{
			vecSkinMeshRes.AddMesh(sc.vecSkinMesh[meshIdx]);
		}
		vecMeshRes.UpdateRes(dx12Base.baseCmdQue.GetCmdList());
		vecSkinMeshRes.UpdateRes(dx12Base.baseCmdQue.GetCmdList());

		//vecMeshRes.ShowInfo();
		dx12Base.baseCmdQue.CloseAndExecute();
		dx12Base.baseCmdQue.FlushCmdQue();

		meshDirty = false;
	}
}

void TestScene001_Base::UpdateMeshRcs(std::shared_ptr<SceneNode> nd, std::vector<int>& vecMeshResIdx, std::vector<int>& vecSkinMeshResIdx)
{
	if (nd->IsRenderNode())
	{
		Cmpt_Mesh* mesh = nd->Get<Cmpt_Mesh>();
		Cmpt_SkinMesh* skinMesh = nd->Get<Cmpt_SkinMesh>();

		if (mesh != nullptr)
		{
			mesh->mSubMeshIdx = CheckRepeat(vecMeshResIdx, mesh->mMeshIdx);
		}
		else if (skinMesh != nullptr)
		{
			skinMesh->mSkinSubMeshIdx = CheckRepeat(vecSkinMeshResIdx, skinMesh->mSkinMeshIdx);
		}
	}
	for (auto childNode : nd->child)
	{
		UpdateMeshRcs(childNode, vecMeshResIdx, vecSkinMeshResIdx);
	}
}

void TestScene001_Base::UpdateAnimRcs(std::shared_ptr<SceneNode> nd, DirectX::XMFLOAT4X4 skinRootWorldMat, std::vector<DirectX::XMFLOAT4X4>* mOffsetMulAnimWorld)
{
	if (nd->Get<Cmpt_SkinRoot>() != nullptr)
	{
		skinRootWorldMat = nd->Get<Cmpt_Trans>()->mWorldMat;
		mOffsetMulAnimWorld = &nd->Get<Cmpt_SkinRoot>()->mOffsetMulAnimWorld;
	}
	if (nd->Get<Cmpt_SkinMesh>() != nullptr)
	{
		nd->Get<Cmpt_SkinMesh>()->mOffsetMulAnimWorld = mOffsetMulAnimWorld;
		nd->Get<Cmpt_SkinMesh>()->mSkinRootWorldMat = skinRootWorldMat;
	}
	for (auto childNode : nd->child)
	{
		UpdateAnimRcs(childNode, skinRootWorldMat, mOffsetMulAnimWorld);
	}
}


void TestScene001_Base::InitDetailRes()
{
	/*Shader InputLayer RootSign PSO*/
	sc.root = std::make_shared<SceneNode>("root",world);
	Set_ShowChildWin();
	Set_ShowMainWinUi();

	cubeMapPso = sdMg.GetCubeMapPso({ baseSwch.GetDesc().Format }, rtv_BackDsv.GetDsvFmt(), dx12Base.GetDevice());
	simpleTexPso = sdMg.GetSimpleTexPso({ baseSwch.GetDesc().Format }, rtv_BackDsv.GetDsvFmt(), dx12Base.GetDevice());
	brdfPso = sdMg.GetBrdfPso({ baseSwch.GetDesc().Format }, rtv_BackDsv.GetDsvFmt(), dx12Base.GetDevice());
	screenQuatPso = sdMg.GetScreenQuatPso({ baseSwch.GetDesc().Format },dx12Base.GetDevice());

	sdMg.sd_ObjTexTabel.AllocHp(hpManager);
	lightCb = sdMg.sd_LightCb.Get();
	pipeLine_RectBlur.SetBlurCore(1.0f);

	RDx12::MathHelper::TransTest();
	MakeMeshRes();
	MakeTexRes();
	MakeMatRes();
	AddNodes();
	SystemOrder();
}

//SNORM：表示归一化处理的有符号整数，这个格式的数据在资源（resource）中解释为有符号整数，在shader中则被解释为（-1.0...1.0）之间的浮点数，如DXGI_FORMAT_R8G8B8A8_SNORM。
//UNORM：表示归一化处理的无符号整数，此种格式数据在资源中被解释为无符号整数，在shader中解释为（0.0...1.0)之间的浮点数，如DXGI_FORMAT_R8G8B8A8_UNORM。以2位无符号整数为例，00，01，10，11（即0，1，2，3），分别对应的浮点数为0.0，1/3，2/3，1.0。
//DXGI_FORMAT_R8G8B8A8_UNORM_SRGB    后面没有SRGB的比如 DXGI_FORMAT_R8G8B8A8_UNORM     color值在srgb的buffer里面会被gpu自动pow0.45 会暗一些 鲜艳一些  这样压一下 叫encode给显示器pow2.2用
void TestScene001_Base::ReSizeOtherRes()
{
	//std::cout << "XXXXXXXXXXXXXXXXXReSizeOtherRes" << std::endl;
	depthMap.Init({ rtv_BackDsv.hCpu_Srv() }, sdMg.sd_DepthMap.RpIdx(), hpManager);
	
	pipeLine_Gbuffer.InitorResize(
		baseSwch.GetDesc().Width, baseSwch.GetDesc().Height,
		{ DXGI_FORMAT_R8G8B8A8_SNORM ,				    //Texture2D gNormalBuffer : register(t0, space2);
		  baseSwch.GetDesc().Format,			    //Texture2D gAlbedoBuffer : register(t1, space2);
		  DXGI_FORMAT_R8G8B8A8_UNORM  },			//Texture2D gMetalRoughAoBuffer : register(t2, space2)
		rtv_BackDsv.GetDsvFmt(),
		sdMg,hpManager);
	
	pipeLine_SSAO.InitorResize(
		baseSwch.GetDesc().Width/2, baseSwch.GetDesc().Height/2,
		DXGI_FORMAT_R8_UNORM,
		sdMg, hpManager);

	pipeLine_RectBlur.InitorResize(
		baseSwch.GetDesc().Width / 2, baseSwch.GetDesc().Height / 2,
		DXGI_FORMAT_R8_UNORM);
	
	pipeLine_Sobel.InitorResize(
		baseSwch.GetDesc().Width,baseSwch.GetDesc().Height,
		DXGI_FORMAT_R8_UNORM);

	for (int i = 0; i < mVecCamLayer[(int)CameraLayer::Main].size(); ++i)
	{
		mVecCamLayer[(int)CameraLayer::Main][i]->Get<Cmpt_Camera>()->SetFrustum(0.1f, 1000, static_cast<float>(baseSwch.GetDesc().Width) / baseSwch.GetDesc().Height, 45);
	}
}

void TestScene001_Base::InputHandle(float deltatime)
{
	if (mVecCamLayer[(int)CameraLayer::Main].size() > 0)
	{
		std::shared_ptr<SceneNode> mainCam = mVecCamLayer[(int)CameraLayer::Main][0];

		DirectX::XMFLOAT3 dPos(0, 0, 0);
		DirectX::XMFLOAT4 dQuat(0, 0, 0, 1);
		Cmpt_Trans* tr = mainCam->Get<Cmpt_Trans>();

		/*WIN32DEBUG(L"\n");
		RDx12::MathHelper::ShowMat(mainCam->Get<Cmpt_Camera>()->mViewMat);
		WIN32DEBUG(L"\n");*/

		if (inputDetect.KeyDownContinued(RDx12::InputKey::IK_W))
		{
			dPos = RDx12::MathHelper::F3VAddScale(dPos, tr->mForward, deltatime * 2);
		}
		if (inputDetect.KeyDownContinued(RDx12::InputKey::IK_S))
		{
			dPos = RDx12::MathHelper::F3VSubScale(dPos, tr->mForward, deltatime * 2);
		}
		if (inputDetect.KeyDownContinued(RDx12::InputKey::IK_A))
		{
			dPos = RDx12::MathHelper::F3VSubScale(dPos, tr->mRight, deltatime * 2);
		}
		if (inputDetect.KeyDownContinued(RDx12::InputKey::IK_D))
		{
			dPos = RDx12::MathHelper::F3VAddScale(dPos, tr->mRight, deltatime * 2);
		}
		if (inputDetect.KeyDownContinued(RDx12::InputKey::IK_Q))
		{
			dPos = RDx12::MathHelper::F3VAddScale(dPos, tr->mUp, deltatime * 2);
		}
		if (inputDetect.KeyDownContinued(RDx12::InputKey::IK_E))
		{
			dPos = RDx12::MathHelper::F3VSubScale(dPos, tr->mUp, deltatime * 2);
		}
		if (inputDetect.KeyDownContinued(RDx12::InputKey::IK_LSHIFT))
		{
			float mouseX = inputDetect.GetMouseData(RDx12::InputMouse::IM_X);
			float mouseY = inputDetect.GetMouseData(RDx12::InputMouse::IM_Y);
			dQuat = RDx12::MathHelper::GetYawPitchQuat(mouseY * deltatime * 20,tr->mRight,mouseX * deltatime * 20,RDx12::MathHelper::Up());
		}
		if (!RDx12::MathHelper::F3Equal(dPos, { 0,0,0 }) || !RDx12::MathHelper::F4Equal(dQuat, { 0,0,0,1 }))
		{
			mainCam->ChangeWorldTrans(dPos, {1,1,1}, dQuat);
			//RDx12::MathHelper::ShowMat(mainCam->Get<Cmpt_Camera>()->mProjMat);
		}

		if (inputDetect.KeyDown(RDx12::InputKey::IK_C))
		{
			std::string command;
			command = "mkdir " + RTmpl::ToLeftPath(GobalPath().ScreenGrapDir);
			system(command.c_str());
			
			// Initialize COM
			CoInitialize(NULL);
			DirectX::SaveWICTextureToFile(
				dx12Base.baseCmdQue.GetCmdQue(),
				rtv_BackBuffer[baseSwch.GetFrameIdx()].res.Get(),
				GUID_ContainerFormatPng,
				(RWin32::Win32_Str2Ws(GobalPath().ScreenGrapDir + "/") + std::to_wstring(gameTimer.TotalTime()) + L".png").c_str(),
				D3D12_RESOURCE_STATE_PRESENT,
				D3D12_RESOURCE_STATE_PRESENT);
			CoUninitialize();
		}
	}
}

/*
与frameRes有关的必须在这里进行每帧更新
参见函数BuildFrameRes()
*/
void TestScene001_Base::Update(float deltatime)
{
	UpdateByMessage();

	UpdateLayer();
	UpdateMesh();
	UpdateAnimRcs(sc.root, RDx12::MathHelper::Identity4x4(), nullptr);

	for (auto& mainCam : mVecCamLayer[(int)CameraLayer::Main])
	{
		mainCam->Get<Cmpt_PassCb>()->PreUpdate(
			DirectX::XMFLOAT2(baseSwch.GetDesc().Width, baseSwch.GetDesc().Height),
			DirectX::XMFLOAT2(1.0f / baseSwch.GetDesc().Width, 1.0f / baseSwch.GetDesc().Height),
			gameTimer.TotalTime(),
			gameTimer.DeltaTime()
		);
	}
	lightCb.Update(frvec.GetNowFrIdx());
	sdMg.sd_ObjTexTabel.Cbv_Update(frvec.GetNowFrIdx());
	pipeLine_RectBlur.Update();
	world.Update();//内部更新了sd_PassCb sd_ObjCb sd_ObjHeapCb
}

void TestScene001_Base::RenderLogic(ID3D12GraphicsCommandList* cl)
{
	//RootCb UtilData
	if (mVecCamLayer[(int)CameraLayer::Main].size() > 0)
	{
		//开始记录命令
		cl->SetGraphicsRootSignature(sdMg.rootSign.Get());

		mVecCamLayer[(int)CameraLayer::Main][0]->Get<Cmpt_PassCb>()->SetGraphicsRootConstantBufferView(cl, frvec);//Pass
		lightCb.SetGraphicsRootConstantBufferView(frvec.GetNowFrIdx(), cl);

		//Prepare For CsuGpuHeap
		hpManager.csuHeap_Gpu.SetByCmdList(0, cl);
		sdMg.sd_ObjTexTabel.SetGraphicsRootDescriptorTable(frvec.GetNowFrIdx(), cl);
		cl->ClearDepthStencilView(rtv_BackDsv.hCpu_Dsv(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

		//Gbuffer渲染 顺势获得物体depth
		{
			pipeLine_Gbuffer.RenderReady(cl);
			{
				//天空盒只渲染Gbuffer中的Albedo
				D3D12_CPU_DESCRIPTOR_HANDLE dsvhCpu = rtv_BackDsv.hCpu_Dsv();
				pipeLine_Gbuffer.SetRenderTarget(cl,1,1,&dsvhCpu);
				{
					vecSkyBox[nowSkyBoxIdx].SetGpuData(cl);
					DrawLayerItemByPso(cl, RenderLayer::Sky, cubeMapPso.Get());
				}
				//物体渲染Gbuffer的所有RTV
				{
					pipeLine_Gbuffer.SetRenderTarget(cl, pipeLine_Gbuffer.GetGbufferNum(), 0, &dsvhCpu);
					DrawLayerItemByPso(cl, RenderLayer::Select, pipeLine_Gbuffer.GetPso());
					DrawLayerSkinItemByPso(cl, RenderLayer::Select, pipeLine_Gbuffer.GetSkinPso());
					
					//Computer Shader
					{
						cl->SetComputeRootSignature(sdMg.rootSign.Get());
						mVecCamLayer[(int)CameraLayer::Main][0]->Get<Cmpt_PassCb>()->SetComputeRootConstantBufferView(cl, frvec);
						{
							pipeLine_Sobel.Render(rtv_BackDsv.res.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, rtv_BackDsv.hCpu_Srv(), cl);
							pipeLine_Sobel.SetGpuData(cl);
						}
					}

					cl->SetGraphicsRootSignature(sdMg.rootSign.Get());//之前设置成了ComputeRootSign，此处要重新设置回GraphicsRootSign
					pipeLine_Gbuffer.SetRenderTarget(cl, pipeLine_Gbuffer.GetGbufferNum(), 0, &dsvhCpu);
					DrawLayerItemByPso(cl, RenderLayer::NotSelect, pipeLine_Gbuffer.GetPso());
					DrawLayerSkinItemByPso(cl, RenderLayer::NotSelect, pipeLine_Gbuffer.GetSkinPso());


				}
			}
			pipeLine_Gbuffer.RenderEnd(cl);
			pipeLine_Gbuffer.SetGpuData(cl);
			depthMap.SetGpuData(0, cl);
		}

		//根据Gbuffer获得的normal depth进行SSAO*/
		{
			pipeLine_SSAO.Render(cl);
			pipeLine_SSAO.SetGpuData(cl);
		}
		
		//Computer Shader
		{
			//Computer需要重新设置rootSign和所有Cbv Srv
			//设置函数替换为SetComputeRootSignature, SetComputeRootDescriptorTable, SetComputeRootConstantBufferView等
			cl->SetComputeRootSignature(sdMg.rootSign.Get());
			mVecCamLayer[(int)CameraLayer::Main][0]->Get<Cmpt_PassCb>()->SetComputeRootConstantBufferView(cl, frvec);
			{
				pipeLine_RectBlur.Render(pipeLine_SSAO.GetRes(), D3D12_RESOURCE_STATE_GENERIC_READ, 1, cl);
				pipeLine_RectBlur.SetGpuData(cl);
			}
		}

		cl->SetGraphicsRootSignature(sdMg.rootSign.Get());//之前设置成了ComputeRootSign，此处要重新设置回GraphicsRootSign
		//之前带有PipeLine的变量都含有RTV，内部自带与RTV大小相配的Viewports ScissorRects，这里回到后缓冲需要设置后缓冲大小的Viewports ScissorRects
		cl->RSSetViewports(1, &m_ViewPort);
		cl->RSSetScissorRects(1, &m_ScissorRect);
		
		//后缓冲渲染
		{
			
			cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rtv_BackBuffer[baseSwch.GetFrameIdx()].res.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
			const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
			cl->ClearRenderTargetView(rtv_BackBuffer[baseSwch.GetFrameIdx()].hCpu_Rtv(), clearColor, 0, nullptr);
			//cl->ClearDepthStencilView(rtv_BackDsv.hCpu_Dsv(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

			//延迟渲染结算，直接在四边形屏幕上采样绘制Gbuffer，并处理延迟对象的光照
			cl->OMSetRenderTargets(1, &(rtv_BackBuffer[baseSwch.GetFrameIdx()].hCpu_Rtv()), true, nullptr);//不需要渲染深度图
			DrawScreenQuatByPso(cl, screenQuatPso.Get());

			//前向渲染
			{
				//cl->OMSetRenderTargets(1, &(rtv_BackBuffer[baseSwch.GetFrameIdx()].hCpu_Rtv()), true, &rtv_BackDsv.hCpu_Dsv());
				//vecSkyBox[nowSkyBoxIdx].mGpuHandler.SetGpuData(0, cl);
				//DrawLayerItemByPso(cl, mVecRenderLayer[(int)RenderLayer::Sky], cubeMapPso.Get());
				//DrawLayerItemByPso(cl, mVecRenderLayer[(int)RenderLayer::NonePbr], simpleTexPso.Get());
				//DrawLayerItemByPso(cl, mVecRenderLayer[(int)RenderLayer::Pbr], brdfPso.Get());
			}
		}
		imMain.Draw(cl);

			//又一个资源屏障，用于确定渲染已经结束可以提交画面去显示了
		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rtv_BackBuffer[baseSwch.GetFrameIdx()].res.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	}
	else
	{
		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rtv_BackBuffer[baseSwch.GetFrameIdx()].res.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		cl->ClearRenderTargetView(rtv_BackBuffer[baseSwch.GetFrameIdx()].hCpu_Rtv(), clearColor, 0, nullptr);
		cl->OMSetRenderTargets(1, &(rtv_BackBuffer[baseSwch.GetFrameIdx()].hCpu_Rtv()), true, nullptr);
		imMain.Draw(cl);
		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(rtv_BackBuffer[baseSwch.GetFrameIdx()].res.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	}
}


void TestScene001_Base::MakeMeshRes()
{
	//顶点的基础数据
	sc.vecPureMesh.emplace_back(RDx12::GeoGen::GetPlane("plane", 1.0f, 1.0f));
	sc.vecPureMesh.emplace_back(RDx12::GeoGen::GetCube("cube", 1.0f, 1.0f,1.0f));
	sc.vecPureMesh.emplace_back(RDx12::GeoGen::GetSphere("sphere", 1.0f, 20, 20));
}

void TestScene001_Base::MakeTexRes()
{
	std::cout << std::endl;
	std::cout << "TestScene001_Base::MakeTexRes()" << std::endl << std::endl;

	//读取文件夹下的所有纹理，创建LdTex资源
	dx12Base.baseCmdQue.FlushCmdQue();
	dx12Base.baseCmdQue.ListReset();

	std::string dirPath = GobalPath().TexturePath;
	std::vector<std::string> filePathVec;
	std::vector<std::string> fileNameVec;
	RWin32::GetFilePathAndName(dirPath, "/", filePathVec, fileNameVec);

	std::cout << std::endl;
	std::cout << "TestScene001_Base::LoadTex()" << std::endl << std::endl;
	for (int i=0;i<filePathVec.size();i++)
	{
		std::string filePath = filePathVec[i];
		std::string fileName = fileNameVec[i];
		if (filePath.find(".dds") != std::string::npos || filePath.find(".jpg") != std::string::npos || filePath.find(".png") != std::string::npos)
		{
			std::cout << filePath << std::endl;
			std::string name = RTmpl::StrReplaceAll<std::string>(filePath, dirPath+"/", "");
			RDx12::LdTex tex(name, filePath, dx12Base.baseCmdQue.GetCmdList(), hpManager);
			ldTexDic[tex.Name()] = tex;
		}
	}
	std::cout << std::endl;

	dx12Base.baseCmdQue.CloseAndExecute();
	dx12Base.baseCmdQue.FlushCmdQue();

	for (auto& kv : ldTexDic)
	{
		kv.second.ReleaseUpload();
	}

	//6张LdTex创建MkTex_Cube资源
	dx12Base.baseCmdQue.FlushCmdQue();
	dx12Base.baseCmdQue.ListReset();

	mkTexCubeDic["IceRiver"] = RDx12::MkTex_Cube("IceRiver", ldTexDic["CubeMap/IceRiver/posx.jpg"].Res(), ldTexDic["CubeMap/IceRiver/negx.jpg"].Res(), ldTexDic["CubeMap/IceRiver/posy.jpg"].Res(), ldTexDic["CubeMap/IceRiver/negy.jpg"].Res(), ldTexDic["CubeMap/IceRiver/posz.jpg"].Res(), ldTexDic["CubeMap/IceRiver/negz.jpg"].Res(),  dx12Base.baseCmdQue.GetCmdList(), hpManager);
	mkTexCubeDic["Bridge"] = RDx12::MkTex_Cube("Bridge", ldTexDic["CubeMap/Bridge/posx.jpg"].Res(), ldTexDic["CubeMap/Bridge/negx.jpg"].Res(), ldTexDic["CubeMap/Bridge/posy.jpg"].Res(), ldTexDic["CubeMap/Bridge/negy.jpg"].Res(), ldTexDic["CubeMap/Bridge/posz.jpg"].Res(), ldTexDic["CubeMap/Bridge/negz.jpg"].Res(),  dx12Base.baseCmdQue.GetCmdList(), hpManager);
	mkTexCubeDic["Bridge2"] = RDx12::MkTex_Cube("Bridge2", ldTexDic["CubeMap/Bridge2/posx.jpg"].Res(), ldTexDic["CubeMap/Bridge2/negx.jpg"].Res(), ldTexDic["CubeMap/Bridge2/posy.jpg"].Res(), ldTexDic["CubeMap/Bridge2/negy.jpg"].Res(), ldTexDic["CubeMap/Bridge2/posz.jpg"].Res(), ldTexDic["CubeMap/Bridge2/negz.jpg"].Res(), dx12Base.baseCmdQue.GetCmdList(), hpManager);
	mkTexCubeDic["ArstaBridge"] = RDx12::MkTex_Cube("ArstaBridge", ldTexDic["CubeMap/ArstaBridge/posx.jpg"].Res(), ldTexDic["CubeMap/ArstaBridge/negx.jpg"].Res(), ldTexDic["CubeMap/ArstaBridge/posy.jpg"].Res(), ldTexDic["CubeMap/ArstaBridge/negy.jpg"].Res(), ldTexDic["CubeMap/ArstaBridge/posz.jpg"].Res(), ldTexDic["CubeMap/ArstaBridge/negz.jpg"].Res(), dx12Base.baseCmdQue.GetCmdList(), hpManager);
	
	dx12Base.baseCmdQue.CloseAndExecute();
	dx12Base.baseCmdQue.FlushCmdQue();
}

void TestScene001_Base::MakeMatRes()
{
	std::cout << std::endl;
	std::cout << "TestScene001_Base::MakeMatRes()" << std::endl << std::endl;
	
	sdMg.sd_ObjTexTabel.AddObjTex(
		"DefaultMat",
		{
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
		},
		dx12Base.GetDevice()
	);
	
	sdMg.sd_ObjTexTabel.AddObjTex(
		"cubeMat1",
		{ 
			&ldTexDic["JPG/WJ/A.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
		},
		dx12Base.GetDevice()
	);

	sdMg.sd_ObjTexTabel.AddObjTex(
		"planeMat1",
		{
			&ldTexDic["JPG/MiShanWu/4.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
		},
		dx12Base.GetDevice()
	);

	sdMg.sd_ObjTexTabel.AddObjTex(
		"pbrMat1",
		{
			&ldTexDic["PBR/rustediron2/rustediron2_basecolor_1k.dds"],
			&ldTexDic["PBR/rustediron2/rustediron2_normal_1k.dds"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["PBR/rustediron2/rustediron2_roughness_1k.dds"],
			&ldTexDic["PBR/rustediron2/rustediron2_metallic_1k.dds"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
		},
		dx12Base.GetDevice()
	);

	sdMg.sd_ObjTexTabel.AddObjTex(
		"pbrMat2",
		{
			&ldTexDic["PBR/metal-ventilation1/metal-ventilation1-albedo_1k.dds"],
			&ldTexDic["PBR/metal-ventilation1/metal-ventilation1-normal-dx_1k.dds"],
			&ldTexDic["default.jpg"],
			&ldTexDic["default.jpg"],
			&ldTexDic["PBR/metal-ventilation1/metal-ventilation1-roughness_1k.dds"],
			&ldTexDic["PBR/metal-ventilation1/metal-ventilation1-metallic_1k.dds"],
			&ldTexDic["PBR/metal-ventilation1/metal-ventilation1-height_1k.dds"],
			&ldTexDic["PBR/metal-ventilation1/metal-ventilation1-ao_1k.dds"],
		},
		dx12Base.GetDevice()
	);

	//创建天空盒资源和材质引用
	for (auto& [mkTexName, mkTexCube] : mkTexCubeDic)
	{
		PipeLine_SkyBox skybox;
		skybox.InitorResize(
			mkTexName, 512,5,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			mkTexCube.hCpu(),
			ldTexDic["PBR/ibl_brdf_lut.png"].hCpu(),
			dx12Base.baseCmdQue,sdMg, hpManager
		);
		vecSkyBox.push_back(skybox);
	}
}

void TestScene001_Base::AddNodes()
{
	std::cout << std::endl;
	std::cout << "TestScene001_Base::BuildLight()" << std::endl << std::endl;

	Light newDirLight; newDirLight.dir = { 0, 0, -1 }; newDirLight.color = { 0.6, 0.6, 0.2 ,1};
	lightCb.data.gDirLight[0].dir = { 0,0,-1 };
	lightCb.data.gDirLight[0].color = { 0.6, 0.6, 0.2 ,1 };
	lightCb.data.gDirLightCount = 1;
	
	std::cout << std::endl;
	std::cout << "TestScene001_Base::BuildCamera()" << std::endl << std::endl;

	auto mainCam = std::make_shared<SceneNode>("mainCam", world);
	Scene::AddChild(sc.root, mainCam);
	SetAsCamera(mainCam, CameraLayer::Main);

	std::cout << std::endl;
	std::cout << "TestScene001_Base::AddNodes()" << std::endl << std::endl;

	auto plane1 = std::make_shared<SceneNode>("plane1", world);
	plane1->SetOffsetTrans({ 110, 5, 200 }, { 40, 25, 0.1 }, { 0,0,0,1 });
	Scene::AddChild(sc.root, plane1);
	SetAsRenderItem(plane1, 0, RenderLayer::NonePbr, 2);

	auto cube1 = std::make_shared<SceneNode>("cube1", world);
	cube1->SetOffsetTrans({ 5, 0, 0 }, { 1, 2, 1 }, RDx12::MathHelper::GetRollPitchYawQuatByDeg(0, 0,0));
	Scene::AddChild(sc.root, cube1);
	SetAsRenderItem(cube1, 1, RenderLayer::NonePbr, 1);

	auto cube2 = std::make_shared<SceneNode>("cube2", world);
	cube2->SetOffsetTrans({ 5, 5, 0 }, { 1, 1, 1 }, RDx12::MathHelper::GetRollPitchYawQuatByDeg(45, 0, 0));
	Scene::AddChild(sc.root, cube2);
	SetAsRenderItem(cube2, 1, RenderLayer::NonePbr, 2);

	auto sphere1 = std::make_shared<SceneNode>("sphere1", world);
	Scene::AddChild(sc.root, sphere1);
	SetAsRenderItem(sphere1, 2, RenderLayer::Pbr, 4);

	auto sphere2 = std::make_shared<SceneNode>("sphere2", world);
	Scene::AddChild(sc.root, sphere2);
	SetAsRenderItem(sphere2, 2, RenderLayer::Sky, 0);
}

void TestScene001_Base::SystemOrder()
{
	/*SystemOrder*/
	world.sysm.Register("CameraUpdate", Cmpt_Camera::Update());
	world.sysm.Register("TransEnd", Cmpt_Trans::End());
	world.sysm.Register("ObjCbUpdate", Cmpt_ObjCb::Update(frvec,dx12Base.GetDevice()));
	world.sysm.Register("PassCbUpdate", Cmpt_PassCb::Update(frvec));
	world.sysm.Register("SkinMeshUpdate",Cmpt_SkinMesh::Update(frvec,dx12Base.GetDevice(), sc.vecSkinMeshBindWorldMat));
	world.sysm.Register("SkinRootUpdate", Cmpt_SkinRoot::Update(&gameTimer, sc.vecSkeleton));
	world.sysm.Register("CmdParserUpdate", Cmpt_CmdParser::Update());
	world.sysm.SetOrder("CameraUpdate", "PassCbUpdate");
	world.sysm.SetOrder("PassCbUpdate", "TransEnd");
	world.sysm.SetOrder("ObjCbUpdate", "TransEnd");
	world.sysm.SetOrder("CmdParserUpdate", "SkinRootUpdate");
	world.sysm.SetOrder("SkinRootUpdate", "SkinMeshUpdate");
	world.sysm.SetOrder("SkinMeshUpdate", "TransEnd");
	world.ShowGraph(RTmpl::ToLeftPath(GobalPath().GraphVizGenPath));
}

void TestScene001_Base::DrawLayerItemByPso(ID3D12GraphicsCommandList* cl, RenderLayer layer, ID3D12PipelineState* pso)
{
	auto mergeMesh = &vecMeshRes;
	auto renderLayer = &mVecRenderLayer[(int)layer];

	cl->SetPipelineState(pso);
	cl->IASetVertexBuffers(0, 1, &(mergeMesh->VertexBufferView()));
	cl->IASetIndexBuffer(&(mergeMesh->IndexBufferView()));

	for (int i = 0; i < renderLayer->size(); i++)
	{
		std::shared_ptr<SceneNode> nd = (*renderLayer)[i];
		if (nd->Get<Cmpt_Mesh>() != nullptr)
		{
			nd->Get<Cmpt_ObjCb>()->SetGraphicsRootConstantBufferView(cl, frvec);
			RDx12::SubMesh submesh = mergeMesh->GetSubMesh(nd->Get<Cmpt_Mesh>()->mSubMeshIdx);
			cl->IASetPrimitiveTopology(submesh.topology);
			cl->DrawIndexedInstanced(submesh.indexCount, 1, submesh.startIndexLocation, submesh.startVertexLocation, 0);
		}
	}
}

void TestScene001_Base::DrawLayerSkinItemByPso(ID3D12GraphicsCommandList* cl, RenderLayer layer, ID3D12PipelineState* pso)
{
	auto mergeMesh = &vecSkinMeshRes;
	auto renderLayer = &mVecRenderLayer[(int)layer];
	
	cl->SetPipelineState(pso);
	cl->IASetVertexBuffers(0, 1, &(mergeMesh->VertexBufferView()));
	cl->IASetIndexBuffer(&(mergeMesh->IndexBufferView()));

	for (int i = 0; i < renderLayer->size(); i++)
	{
		std::shared_ptr<SceneNode> nd = (*renderLayer)[i];
		if (nd->Get<Cmpt_SkinMesh>() != nullptr)
		{
			nd->Get<Cmpt_ObjCb>()->SetGraphicsRootConstantBufferView(cl, frvec);
			nd->Get<Cmpt_SkinMesh>()->SetGraphicsRootConstantBufferView(cl, frvec);
			RDx12::SubMesh submesh = mergeMesh->GetSubMesh(nd->Get<Cmpt_SkinMesh>()->mSkinSubMeshIdx);
			cl->IASetPrimitiveTopology(submesh.topology);
			cl->DrawIndexedInstanced(submesh.indexCount, 1, submesh.startIndexLocation, submesh.startVertexLocation, 0);
		}
	}
}

void TestScene001_Base::DrawScreenQuatByPso(ID3D12GraphicsCommandList* cl, ID3D12PipelineState* pso)
{
	cl->SetPipelineState(pso);
	cl->IASetVertexBuffers(0, 0, nullptr);
	cl->IASetIndexBuffer(nullptr);
	cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cl->DrawInstanced(6, 1, 0, 0);
}

