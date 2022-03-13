#pragma once
#include "../../../../../3rdParty/Dx12Dragon/d3dx12.h"
#include "../../../../../3rdParty/Dx12Dragon/DDSTextureLoader.h"
#include  "../../../../../3rdParty/DirectXTex/ScreenGrab/ScreenGrab12.h"
#include  "../../../../../3rdParty/DirectXTex/WICTextureLoader/WICTextureLoader12.h"
#include "../../../../../Include/RDx12/ResCreate/DescHeap/HeapManager.h"

namespace RDx12
{
	//LoadTex：texture.dds load by DDSTextureLoader.h
	class LdTex
	{
	private:
		std::string name;		//有名字便于存放在词典中
		std::string filePath;
		Microsoft::WRL::ComPtr<ID3D12Resource> res = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> uploadRes = nullptr;
		std::shared_ptr<HeapAllocation> hpAlloc;
		void Bind(HeapManager& hpManager);
	public:
		LdTex() {};
		LdTex(std::string name, std::string filePath, ID3D12GraphicsCommandList* cmdList, HeapManager& hpManager);//filePath-filePrefix=name
		std::string Name() { return name; }
		std::string FilePath() { return  filePath; }
		ID3D12Resource* Res() { return res.Get(); }
		D3D12_CPU_DESCRIPTOR_HANDLE hCpu();
		void ReleaseUpload() { uploadRes.Reset(); }
	};

	//通过复制6张ldTex构成一个CubeMap
	class MkTex_Cube
	{
	private:
		std::string name;
		Microsoft::WRL::ComPtr<ID3D12Resource> res = nullptr;
		std::shared_ptr<HeapAllocation> hpAlloc;
		void Bind(HeapManager& hpManager);
		void CopySrcFillDstOneDepth(ID3D12Resource* dstRes, UINT depthOrArrayIdx, ID3D12Resource* srcRes, ID3D12Device4* dev, ID3D12GraphicsCommandList* cl);
	public:
		MkTex_Cube() {};
		MkTex_Cube(std::string name, ID3D12Resource* posx, ID3D12Resource* negx, ID3D12Resource* posy, ID3D12Resource* negy, ID3D12Resource* posz, ID3D12Resource* negz, ID3D12GraphicsCommandList* cl,HeapManager& hpManager);
		std::string Name() { return name; }
		ID3D12Resource* Res() { return res.Get(); }	
		D3D12_CPU_DESCRIPTOR_HANDLE hCpu();
	};

	
	class RtvTex
	{
	private:
		std::shared_ptr<HeapAllocation> rtvAlloc;
		std::shared_ptr<HeapAllocation> srvAlloc;
		void Bind(HeapManager& hpManager);
	public:
		Microsoft::WRL::ComPtr<ID3D12Resource> res = nullptr;
		RtvTex() {};
		void Create(UINT width,UINT height, DXGI_FORMAT fmt, HeapManager& hpManager,std::vector<float> clearColor);
		void Create(IDXGISwapChain* swch,UINT idx, HeapManager& hpManager);
		D3D12_CPU_DESCRIPTOR_HANDLE hCpu_Rtv();
		D3D12_CPU_DESCRIPTOR_HANDLE hCpu_Srv();
	};

	class RtvTex_Cube
	{
	private:
		std::shared_ptr<HeapAllocation> rtvAlloc;
		std::shared_ptr<HeapAllocation> srvAlloc;
		void Bind(HeapManager& hpManager);
	public:
		Microsoft::WRL::ComPtr<ID3D12Resource> res = nullptr;
		RtvTex_Cube() {};
		void Create(UINT width, UINT height, DXGI_FORMAT fmt, HeapManager& hpManager);
		ID3D12Resource* Res() { return res.Get(); }
		D3D12_CPU_DESCRIPTOR_HANDLE hCpu_Rtv(int num);
		D3D12_CPU_DESCRIPTOR_HANDLE hCpu_Srv();
	};

	class MipTex_Cube
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> res = nullptr;
		std::shared_ptr<HeapAllocation> hpAlloc;
		void Bind(HeapManager& hpManager);
		void CopySrcFillDstOneMip(ID3D12Resource* dstRes, UINT mipLevel, ID3D12Resource* srcRes, ID3D12GraphicsCommandList* cl);
	public:
		MipTex_Cube(){}
		void Create(std::vector<ID3D12Resource*> rtvCubeMapRes, ID3D12GraphicsCommandList* cl, HeapManager& hpManager);
		D3D12_CPU_DESCRIPTOR_HANDLE hCpu();
	};

	class DsvTex
	{
	private:
		std::shared_ptr<HeapAllocation> dsvAlloc;
		std::shared_ptr<HeapAllocation> srvAlloc;
		void Bind(HeapManager& hpManager);
		DXGI_FORMAT resFmt = DXGI_FORMAT_R24G8_TYPELESS;
		DXGI_FORMAT dsvFmt = DXGI_FORMAT_D24_UNORM_S8_UINT;
		DXGI_FORMAT srvFmt = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	public:
		Microsoft::WRL::ComPtr<ID3D12Resource> res = nullptr;
		DsvTex() {};
		void Create(UINT width, UINT height, HeapManager& hpManager);
		D3D12_CPU_DESCRIPTOR_HANDLE hCpu_Dsv();
		D3D12_CPU_DESCRIPTOR_HANDLE hCpu_Srv();
		DXGI_FORMAT GetResFmt() { return resFmt; }
		DXGI_FORMAT GetDsvFmt() { return dsvFmt; }
		DXGI_FORMAT GetSrvFmt() { return srvFmt; }
	};

	class ComputeTex
	{
	private:
		std::shared_ptr<HeapAllocation> csuAlloc;
		void Bind(HeapManager& hpManager);
	public:
		Microsoft::WRL::ComPtr<ID3D12Resource> res = nullptr;
		ComputeTex() {};
		void Create(UINT width, UINT height, DXGI_FORMAT fmt, HeapManager& hpManager);
		D3D12_CPU_DESCRIPTOR_HANDLE hCpu_Srv();
		D3D12_CPU_DESCRIPTOR_HANDLE hCpu_Uav();
	};
	
}
