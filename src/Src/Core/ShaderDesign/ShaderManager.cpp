#include "../../../Include/Core/ShaderDesign/ShaderManager.h"

Tb_ObjTexSet::Tb_ObjTexSet(int frameNum) :Sign_RootTable(frameNum) {
	for (auto& srvTex : objTexSet)
	{
		vecRange.push_back(srvTex.range.desRange);
	}
}

void Tb_ObjTexSet::InitRootParam(std::vector<CD3DX12_ROOT_PARAMETER>& vecRootParam, ID3D12Device4* dev)
{
	sd_ObjTexCbSet.InitRootParam(vecRootParam, dev);
	Sign_RootTable::InitRootParam(vecRootParam);
}

void Tb_ObjTexSet::AllocHp(RDx12::HeapManager& hpManager)
{
	int total = 0;
	for (auto& srvTex : objTexSet)
	{
		total += srvTex.range.DesNum();
	}
	hpAlloc = hpManager.csuHeap_Gpu.Get(total);
}

void Tb_ObjTexSet::SetGraphicsRootDescriptorTable(int frameIdx, ID3D12GraphicsCommandList* cl)
{
	cl->SetGraphicsRootDescriptorTable(rpIdx, hpAlloc->hGpu[0]);
	sd_ObjTexCbSet.SetGraphicsRootShaderResourceView(frameIdx, cl);
}

void Tb_ObjTexSet::Cbv_Update(int frameIdx)
{
	if (frameDirty > 0)
	{
		sd_ObjTexCbSet.Update(frameIdx);
		frameDirty--;
	}
}

void Tb_ObjTexSet::Cbv_Dirty()
{
	frameDirty = frameNum;
}

void Tb_ObjTexSet::AddObjTex(
	std::string name,
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> hCpuVec,
	ID3D12Device4* dev)
{
	assert(hCpuVec.size() == Navigation_ObjTexCb.size());

	ObjTexCb objTexCb;
	for (int i = 0; i < hCpuVec.size(); i++)
	{
		objTexCb.hCpuIdx[i] = CheckRepeat(hCpuVec[i], i, dev);
	}
	objTexNameVec.push_back(name);
	sd_ObjTexCbSet.vecData.push_back(objTexCb);
	Cbv_Dirty();
}

void Tb_ObjTexSet::AddObjTex(
	std::string name,
	std::vector<RDx12::LdTex*> ldTexVec,
	ID3D12Device4* dev)
{
	assert(ldTexVec.size() == Navigation_ObjTexCb.size());

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> hCpuVec;
	std::vector<std::string> realSource;
	for (auto& ldTexPtr : ldTexVec)
	{
		hCpuVec.push_back(ldTexPtr->hCpu());
		realSource.push_back(ldTexPtr->Name());
	}

	objTexRealPath.push_back(realSource);

	Tb_ObjTexSet::AddObjTex(
		name,
		hCpuVec,
		dev);
}

void Tb_ObjTexSet::ModifyObjTex(
	int objTexIdx,
	std::string name,
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>hCpuVec,
	ID3D12Device4* dev)
{
	assert(hCpuVec.size() == Navigation_ObjTexCb.size());

	ObjTexCb objTexCb;
	for (int i = 0; i < hCpuVec.size(); i++)
	{
		objTexCb.hCpuIdx[i] = CheckRepeat(hCpuVec[i], i, dev);
	}
	objTexNameVec[objTexIdx] = name;
	sd_ObjTexCbSet.vecData[objTexIdx] = objTexCb;
	Cbv_Dirty();
}

void Tb_ObjTexSet::ModifyObjTex(
	int objTexIdx,
	std::string name,
	std::vector<RDx12::LdTex*> ldTexVec,
	ID3D12Device4* dev)
{
	assert(ldTexVec.size() == Navigation_ObjTexCb.size());

	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> hCpuVec;
	std::vector<std::string> realSource;
	for (auto& ldTexPtr : ldTexVec)
	{
		hCpuVec.push_back(ldTexPtr->hCpu());
		realSource.push_back(ldTexPtr->Name());
	}

	objTexRealPath[objTexIdx]=realSource;

	Tb_ObjTexSet::ModifyObjTex(
		objTexIdx,
		name,
		hCpuVec,
		dev);
}

void Tb_ObjTexSet::RemoveObjTex(int objTexIdx)
{
	objTexNameVec.erase(objTexIdx + objTexNameVec.begin());
	objTexRealPath.erase(objTexIdx + objTexRealPath.begin());
	sd_ObjTexCbSet.vecData.erase(objTexIdx + sd_ObjTexCbSet.vecData.begin());
	Cbv_Dirty();
}

int Tb_ObjTexSet::GetObjTexNum()
{
	return objTexNameVec.size();
}

std::string* Tb_ObjTexSet::GetObjTexName(int objTexIdx)
{
	return &objTexNameVec[objTexIdx];
}

std::vector<std::string> Tb_ObjTexSet::GetObjTexPath(int objTexIdx)
{
	return objTexRealPath[objTexIdx];
}

D3D12_CPU_DESCRIPTOR_HANDLE Tb_ObjTexSet::GetMaphCpu(int objTexIdx, int objTexType)
{
	ObjTexCb objTexCb = sd_ObjTexCbSet.vecData[objTexIdx];
	int mapIdx = objTexCb.hCpuIdx[objTexType];
	return objTexSet[objTexType].vechCpu[mapIdx];
}

/*
检查该图片的hCpu是否存在于类型为objTexType的vechCpu中，
若存在，返回其下标
不存在，推入hCpu并更新当前类型的hp范围
*/
int Tb_ObjTexSet::CheckRepeat(D3D12_CPU_DESCRIPTOR_HANDLE hCpu, int objTexType, ID3D12Device4* dev)
{
	for (int i = 0; i < objTexSet[objTexType].vechCpu.size(); i++)
	{
		if (hCpu.ptr == objTexSet[objTexType].vechCpu[i].ptr)
		{
			return i;
		}
	}
	objTexSet[objTexType].vechCpu.push_back(hCpu);
	SrvTex_Update(objTexType, dev);
	return objTexSet[objTexType].vechCpu.size() - 1;
}

void Tb_ObjTexSet::SrvTex_Update(int objTexType, ID3D12Device4* dev)
{
	int offset = 0;
	for (int i = 0; i < objTexType; i++)
	{
		offset += objTexSet[i].range.DesNum();
	}

	for (int i = 0; i < objTexSet[objTexType].range.DesNum(); i++)
	{
		if (i < objTexSet[objTexType].vechCpu.size())
			dev->CopyDescriptorsSimple(1, hpAlloc->hCpu[i + offset], objTexSet[objTexType].vechCpu[i], D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		else
			break;
	}
}



Tb_Blur::Tb_Blur(int frameNum) :Sign_RootTable(frameNum) {
	vecRange.push_back(sd_BlurCoreCb.desRange);
	vecRange.push_back(sd_BlurInput.desRange);
	vecRange.push_back(sd_BlurOutput.desRange);
}

void Tb_Blur::InitRootParam(std::vector<CD3DX12_ROOT_PARAMETER>& vecRootParam, ID3D12Device4* dev)
{
	Sign_RootTable::InitRootParam(vecRootParam);
	sd_BlurCoreCb.InitRes(dev);
}

Tb_Sobel::Tb_Sobel(int frameNum) :Sign_RootTable(frameNum) {
	vecRange.push_back(sd_SobelInput.desRange);
	vecRange.push_back(sd_SobelOutput.desRange);
}

ShaderManager::ShaderManager(int frameNum, ID3D12Device4* dev) :frameNum(frameNum)
{
	std::cout <<"ShaderManager Constructor"<< std::endl;
	rootSign = GetRootSign(dev);

	inpLayVec =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	skinInpLayVec =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 72, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	const D3D_SHADER_MACRO skinnedDefines[] =
	{
		"SKINNED", "1",
		NULL, NULL
	};

	shaderDic["Vs_CubeMap"] = RDx12::CompileShader(L"./Assets/shader/Vs_CubeMap.hlsl", nullptr, "VSMain", "vs_5_1");
	shaderDic["Vs_Gbuffer"] = RDx12::CompileShader(L"./Assets/shader/Vs_Gbuffer.hlsl", nullptr, "VSMain", "vs_5_1");
	shaderDic["Vs_Gbuffer_Skin"] = RDx12::CompileShader(L"./Assets/shader/Vs_Gbuffer.hlsl", skinnedDefines, "VSMain", "vs_5_1");
	shaderDic["Vs_Default"] = RDx12::CompileShader(L"./Assets/shader/Vs_Default.hlsl", nullptr, "VSMain", "vs_5_1");
	shaderDic["Vs_ScreenQuat"] = RDx12::CompileShader(L"./Assets/shader/Vs_ScreenQuat.hlsl", nullptr, "VSMain", "vs_5_1");

	shaderDic["Ps_CubeMap"] = RDx12::CompileShader(L"./Assets/shader/Ps_CubeMap.hlsl", nullptr, "PSMain", "ps_5_1");
	shaderDic["Ps_IBL_Diffuse"] = RDx12::CompileShader(L"./Assets/shader/Ps_IBL_Diffuse.hlsl", nullptr, "PSMain", "ps_5_1");
	shaderDic["Ps_IBL_PreFiltered"] = RDx12::CompileShader(L"./Assets/shader/Ps_IBL_PreFiltered.hlsl", nullptr, "PSMain", "ps_5_1");
	shaderDic["Ps_Brdf"] = RDx12::CompileShader(L"./Assets/shader/Ps_Brdf.hlsl", nullptr, "PSMain", "ps_5_1");
	shaderDic["Ps_SimpleTex"] = RDx12::CompileShader(L"./Assets/shader/Ps_SimpleTex.hlsl", nullptr, "PSMain", "ps_5_1");
	shaderDic["Ps_Gbuffer"] = RDx12::CompileShader(L"./Assets/shader/Ps_Gbuffer.hlsl", nullptr, "PSMain", "ps_5_1");
	shaderDic["Ps_Gbuffer_Skin"] = RDx12::CompileShader(L"./Assets/shader/Ps_Gbuffer.hlsl", skinnedDefines, "PSMain", "ps_5_1");
	shaderDic["Ps_Ssao"] = RDx12::CompileShader(L"./Assets/shader/Ps_Ssao.hlsl", nullptr, "PSMain", "ps_5_1");
	shaderDic["Ps_ScreenQuat"] = RDx12::CompileShader(L"./Assets/shader/Ps_ScreenQuat.hlsl", nullptr, "PSMain", "ps_5_1");

	shaderDic["Cs_HorzBlur"] = RDx12::CompileShader(L"./Assets/shader/Cs_Blur.hlsl", nullptr, "CSMain_HorzBlur", "cs_5_1");
	shaderDic["Cs_VertBlur"] = RDx12::CompileShader(L"./Assets/shader/Cs_Blur.hlsl", nullptr, "CSMain_VertBlur", "cs_5_1");
	shaderDic["Cs_Sobel"] = RDx12::CompileShader(L"./Assets/shader/Cs_Sobel.hlsl", nullptr, "CSMain", "cs_5_1");

	std::cout << std::endl;
}

Microsoft::WRL::ComPtr<ID3D12RootSignature> ShaderManager::GetRootSign(ID3D12Device4* dev)
{
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSign;

	std::vector<CD3DX12_ROOT_PARAMETER> rootParamVec;
	sd_PassCb.InitRootParam(rootParamVec, dev);
	sd_ObjCb.InitRootParam(rootParamVec, dev);
	sd_SkinnedCb.InitRootParam(rootParamVec, dev);
	sd_LightCb.InitRootParam(rootParamVec, dev);
	sd_ObjTexTabel.InitRootParam(rootParamVec, dev);
	sd_BlurTable.InitRootParam(rootParamVec,dev);
	sd_SobelTable.InitRootParam(rootParamVec);
	sd_CubeMap.SetAsTable(rootParamVec);
	sd_DepthMap.SetAsTable(rootParamVec);
	sd_SsaoMap.SetAsTable(rootParamVec);
	sd_Gbuffer.SetAsTable(rootParamVec);
	

	CD3DX12_ROOT_SIGNATURE_DESC rootSignDesc = {};
	rootSignDesc.NumParameters = rootParamVec.size();
	rootSignDesc.pParameters = rootParamVec.data();
	rootSignDesc.NumStaticSamplers = staticSample.size();
	rootSignDesc.pStaticSamplers = staticSample.data();
	rootSignDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	rootSign = RDx12::CreateRootSign(dev, rootSignDesc);

	return rootSign;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> ShaderManager::GetSimpleTexPso(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt, ID3D12Device4* dev)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = GetDefaultPsoDesc(rtvFmtVec, depthFmt);

	psoDesc.VS = CD3DX12_SHADER_BYTECODE(shaderDic["Vs_Default"].Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(shaderDic["Ps_SimpleTex"].Get());

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	RWin32::ThrowIfFailed(dev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));

	return pso;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> ShaderManager::GetCubeMapPso(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt, ID3D12Device4* dev)
{

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = GetDefaultPsoDesc(rtvFmtVec, depthFmt);

	psoDesc.VS = CD3DX12_SHADER_BYTECODE(shaderDic["Vs_CubeMap"].Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(shaderDic["Ps_CubeMap"].Get());

	// The camera is inside the sky sphere, so just turn off culling.
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//

	// Make sure the depth function is LESS_EQUAL and not just LESS.  
	// Otherwise, the normalized depth values at z = 1 (NDC) will 
	// fail the depth test if the depth buffer was cleared to 1.
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	RWin32::ThrowIfFailed(dev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));

	return pso;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> ShaderManager::GetIBL_DiffusePso(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt, ID3D12Device4* dev)
{

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = GetDefaultPsoDesc(rtvFmtVec, depthFmt);

	psoDesc.VS = CD3DX12_SHADER_BYTECODE(shaderDic["Vs_CubeMap"].Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(shaderDic["Ps_IBL_Diffuse"].Get());

	// The camera is inside the sky sphere, so just turn off culling.
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//

	// Make sure the depth function is LESS_EQUAL and not just LESS.  
	// Otherwise, the normalized depth values at z = 1 (NDC) will 
	// fail the depth test if the depth buffer was cleared to 1.
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	RWin32::ThrowIfFailed(dev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));

	return pso;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> ShaderManager::GetIBL_PreFilteredPso(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt, ID3D12Device4* dev)
{

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = GetDefaultPsoDesc(rtvFmtVec, depthFmt);

	psoDesc.VS = CD3DX12_SHADER_BYTECODE(shaderDic["Vs_CubeMap"].Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(shaderDic["Ps_IBL_PreFiltered"].Get());

	// The camera is inside the sky sphere, so just turn off culling.
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//

	// Make sure the depth function is LESS_EQUAL and not just LESS.  
	// Otherwise, the normalized depth values at z = 1 (NDC) will 
	// fail the depth test if the depth buffer was cleared to 1.
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	RWin32::ThrowIfFailed(dev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));

	return pso;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> ShaderManager::GetBrdfPso(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt, ID3D12Device4* dev)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = GetDefaultPsoDesc(rtvFmtVec, depthFmt);

	psoDesc.VS = CD3DX12_SHADER_BYTECODE(shaderDic["Vs_Default"].Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(shaderDic["Ps_Brdf"].Get());

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	RWin32::ThrowIfFailed(dev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));

	return pso;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> ShaderManager::GetGbufferPso(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt, ID3D12Device4* dev)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = GetDefaultPsoDesc(rtvFmtVec, depthFmt);

	psoDesc.VS = CD3DX12_SHADER_BYTECODE(shaderDic["Vs_Gbuffer"].Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(shaderDic["Ps_Gbuffer"].Get());

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	RWin32::ThrowIfFailed(dev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));

	return pso;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> ShaderManager::GetGbufferSkinPso(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt, ID3D12Device4* dev)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = GetDefaultPsoDesc(rtvFmtVec, depthFmt);

	psoDesc.VS = CD3DX12_SHADER_BYTECODE(shaderDic["Vs_Gbuffer_Skin"].Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(shaderDic["Ps_Gbuffer_Skin"].Get());
	psoDesc.InputLayout = { skinInpLayVec.data(),(UINT)skinInpLayVec.size() };

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	RWin32::ThrowIfFailed(dev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));

	return pso;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> ShaderManager::GetScreenQuatPso(std::vector<DXGI_FORMAT> rtvFmtVec,ID3D12Device4* dev)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = GetDefaultPsoDesc(rtvFmtVec, DXGI_FORMAT_UNKNOWN);
	psoDesc.DepthStencilState.DepthEnable = false;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(shaderDic["Vs_ScreenQuat"].Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(shaderDic["Ps_ScreenQuat"].Get());

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	RWin32::ThrowIfFailed(dev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));

	return pso;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> ShaderManager::GetSsaoPso(std::vector<DXGI_FORMAT> rtvFmtVec,ID3D12Device4* dev)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = GetDefaultPsoDesc(rtvFmtVec, DXGI_FORMAT_UNKNOWN);
	psoDesc.DepthStencilState.DepthEnable = false;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(shaderDic["Vs_ScreenQuat"].Get());
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(shaderDic["Ps_Ssao"].Get());

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	RWin32::ThrowIfFailed(dev->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso)));

	return pso;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> ShaderManager::GetHorzBlurPso(ID3D12Device4* dev)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootSign.Get();
	psoDesc.CS = CD3DX12_SHADER_BYTECODE(shaderDic["Cs_HorzBlur"].Get());
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	RWin32::ThrowIfFailed(dev->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pso)));

	return pso;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> ShaderManager::GetVertBlurPso(ID3D12Device4* dev)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootSign.Get();
	psoDesc.CS = CD3DX12_SHADER_BYTECODE(shaderDic["Cs_VertBlur"].Get());
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	RWin32::ThrowIfFailed(dev->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pso)));

	return pso;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> ShaderManager::GetSobelPso(ID3D12Device4* dev)
{
	D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootSign.Get();
	psoDesc.CS = CD3DX12_SHADER_BYTECODE(shaderDic["Cs_Sobel"].Get());
	psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
	RWin32::ThrowIfFailed(dev->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&pso)));

	return pso;
}

D3D12_GRAPHICS_PIPELINE_STATE_DESC ShaderManager::GetDefaultPsoDesc(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootSign.Get();
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.InputLayout = { inpLayVec.data(),(UINT)inpLayVec.size() };
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	assert(rtvFmtVec.size() <= 8);
	psoDesc.NumRenderTargets = rtvFmtVec.size();
	for (int i = 0; i < rtvFmtVec.size(); i++)
	{
		psoDesc.RTVFormats[i] = rtvFmtVec[i];
	}

	psoDesc.DSVFormat = depthFmt;
	psoDesc.SampleDesc.Count = 1;
	return psoDesc;
}

