#pragma once
#include <RDx12/ResCreate/ShaderSign/SdSign_Cbv.h>
#include <RDx12/ResCreate/ShaderSign/SdSign_Srv.h>
#include <RDx12/ResCreate/Res/Sample/StaticSample.h>
#include <RWin32/Win32Char.h>
#include "Design_Base.h"
#include <wrl.h>
#include <RDx12/ResCreate/Res/Texture/Texture.h>

/*
设计rootSign，shader寄存器布局，并编译每个Shader生成Pso
*/

class Tb_ObjTexSet :public RDx12::Sign_RootTable
{
public:
	Tb_ObjTexSet(int frameNum);
	void InitRootParam(std::vector<CD3DX12_ROOT_PARAMETER>& vecRootParam, ID3D12Device4* dev);
	void AllocHp(RDx12::HeapManager& hpManager);
	void SetGraphicsRootDescriptorTable(int frameIdx, ID3D12GraphicsCommandList* cl);
	void Cbv_Update(int frameIdx);
	void AddObjTex(std::string name, std::vector<RDx12::LdTex*> ldTexVec, ID3D12Device4* dev);
	void ModifyObjTex(int objTexIdx, std::string name, std::vector<RDx12::LdTex*> ldTexVec, ID3D12Device4* dev);
	void RemoveObjTex(int objTexIdx);
	
	int GetObjTexNum();
	std::string* GetObjTexName(int objTexIdx);
	std::vector<std::string> GetObjTexPath(int objTexIdx);
	D3D12_CPU_DESCRIPTOR_HANDLE Tb_ObjTexSet::GetMaphCpu(int objTexIdx, int objTexType);

	void Cbv_Dirty();
private:

	/*Cbv*/
	std::vector<std::string> objTexNameVec;
	RDx12::Sign_StructuredBuffer<ObjTexCb> sd_ObjTexCbSet{ 0,15,500,frameNum };
	std::vector<std::vector<std::string>> objTexRealPath;
	int frameDirty;

	/*
	SrvTexSet
	相当于一个二维数组，每行储存同类Map的hCpu
	*/
	std::vector<RDx12::SrvTexSet> objTexSet
	{
		{"gAlbedoMap",{0,16,20},{} },
		{"gNormalMap",{0,17,20},{} },
		{"gDisplacementMap",{0,18,20},{} },
		{"gSpecularMap",{0,19,20},{} },
		{"gRoughMap",{0,20,20},{} },
		{"gMetalMap",{0,21,20},{} },
		{"gHeightMap",{0,22,20},{} },
		{"gAOMap",{0,23,20},{} }
	};
	std::shared_ptr<RDx12::HeapAllocation> hpAlloc;

	void AddObjTex(std::string name, std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> hCpuVec, ID3D12Device4* dev);
	void ModifyObjTex(int objTexIdx, std::string name, std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> hCpuVec, ID3D12Device4* dev);
	int CheckRepeat(D3D12_CPU_DESCRIPTOR_HANDLE hCpu, int srvTexIdx, ID3D12Device4* dev);
	void SrvTex_Update(int srvTexIdx, ID3D12Device4* dev);
};


class Tb_Blur :public RDx12::Sign_RootTable
{
public:
	Tb_Blur(int frameNum);
	void InitRootParam(std::vector<CD3DX12_ROOT_PARAMETER>& vecRootParam, ID3D12Device4* dev);
	RDx12::FrameResAlloc_Range<BlurCoreCb> Get() { return sd_BlurCoreCb.Get(); }
private:
	RDx12::DesRange_Cbv<BlurCoreCb> sd_BlurCoreCb{ 3,0,1,10,frameNum };
	RDx12::DesRange_Srv sd_BlurInput{ 0, 2, 1 };
	RDx12::DesRange_Uav sd_BlurOutput{ 0, 0 , 1 };
};

class Tb_Sobel :public RDx12::Sign_RootTable
{
public:
	Tb_Sobel(int frameNum);
private:
	RDx12::DesRange_Srv sd_SobelInput{ 1, 2, 1 };
	RDx12::DesRange_Uav sd_SobelOutput{ 1, 0 , 1 };
};

class ShaderManager
{
public:
	int frameNum = 0;
	RDx12::Sign_RootCb<PassCb> sd_PassCb{ 0, 0, 10, frameNum };
	RDx12::Sign_RootCb<ObjCb> sd_ObjCb{ 1, 0, 500, frameNum };
	RDx12::Sign_RootCb<LightCb> sd_LightCb{ 2, 0, 10, frameNum };
	RDx12::Sign_RootCb<SkinnedCb> sd_SkinnedCb{ 4, 0, 50, frameNum };
	Tb_ObjTexSet sd_ObjTexTabel{ frameNum };
	
	RDx12::SdSign_Srv sd_CubeMap{ "CubeMap", 0, 0, 4 };	
	RDx12::SdSign_Srv sd_DepthMap{ "DepthMap", 0, 1, 1 }; RDx12::SdSign_Srv sd_SsaoMap{ "SSaoMap", 1, 1, 1 };
	RDx12::SdSign_Srv sd_Gbuffer{ "Gbuffer", 2, 1, 3 };
	
	//Cs_Design
	Tb_Blur sd_BlurTable{ frameNum };
	Tb_Sobel sd_SobelTable{ frameNum };

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> staticSample = RDx12::GetStaticSamplers();
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSign;
	std::vector<D3D12_INPUT_ELEMENT_DESC> inpLayVec;
	std::vector<D3D12_INPUT_ELEMENT_DESC> skinInpLayVec;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> shaderDic;

	ShaderManager(int frameNum, ID3D12Device4* dev);
	Microsoft::WRL::ComPtr<ID3D12RootSignature> GetRootSign(ID3D12Device4* dev);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetSimpleTexPso(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt, ID3D12Device4* dev);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetCubeMapPso(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt, ID3D12Device4* dev);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetIBL_DiffusePso(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt, ID3D12Device4* dev);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetIBL_PreFilteredPso(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt, ID3D12Device4* dev);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetBrdfPso(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt, ID3D12Device4* dev);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetGbufferPso(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt, ID3D12Device4* dev);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetGbufferSkinPso(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt, ID3D12Device4* dev);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetScreenQuatPso(std::vector<DXGI_FORMAT> rtvFmtVec,ID3D12Device4* dev);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetSsaoPso(std::vector<DXGI_FORMAT> rtvFmtVec,ID3D12Device4* dev);

	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetHorzBlurPso(ID3D12Device4* dev);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetVertBlurPso(ID3D12Device4* dev);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> GetSobelPso(ID3D12Device4* dev);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC GetDefaultPsoDesc(std::vector<DXGI_FORMAT> rtvFmtVec, DXGI_FORMAT depthFmt);
};