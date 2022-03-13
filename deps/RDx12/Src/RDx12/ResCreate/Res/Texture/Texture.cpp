#include "../Include/RDx12/ResCreate/Res/Texture/Texture.h"
#include "../Include/RDx12/Util/D12Util.h"
#include <RWin32/Win32Debug.h>
#include <RTmpl/STLHelp.h>
#include <RWin32/Win32Char.h>
#include <cassert>

namespace RDx12
{
	void LdTex::Bind(HeapManager& hpManager)
	{
		if (hpAlloc == nullptr) { hpAlloc = hpManager.csuHeap_Cpu.Get(1); }

		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Format = res->GetDesc().Format;
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip = 0;
		desc.Texture2D.MipLevels = res->GetDesc().MipLevels;
		desc.Texture2D.ResourceMinLODClamp = 0.0f;

		hpManager.GetDev()->CreateShaderResourceView(res.Get(), &desc, hpAlloc->hCpu[0]);
	}

	LdTex::LdTex(std::string name, std::string filePath, ID3D12GraphicsCommandList* cmdList, HeapManager& hpManager) :name(name), filePath(filePath)
	{
		if (filePath.find(".dds") != std::string::npos)
		{
			RWin32::ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(
				hpManager.GetDev(),
				cmdList, RWin32::Win32_Str2Ws(filePath).c_str(),
				res, uploadRes));
		}
		else
		{
			CoInitialize(NULL);
			std::unique_ptr<uint8_t[]> wicData;
			D3D12_SUBRESOURCE_DATA subresource;
			RWin32::ThrowIfFailed(DirectX::LoadWICTextureFromFile(
				hpManager.GetDev(),
				RWin32::Win32_Str2Ws(filePath).c_str(),
				res.GetAddressOf(),
				wicData,
				subresource));
			CoUninitialize();
			RDx12::UploadToDefaultTexByData(hpManager.GetDev(), cmdList, res, uploadRes, subresource);
		}

		Bind(hpManager);
		//ShowResInfo(res.Get());
	}

	D3D12_CPU_DESCRIPTOR_HANDLE LdTex::hCpu()
	{
		return hpAlloc->hCpu[0];
	}




	void MkTex_Cube::Bind(HeapManager& hpManager)
	{
		if (hpAlloc == nullptr) { hpAlloc = hpManager.csuHeap_Cpu.Get(1); }

		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Format = res->GetDesc().Format;
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MostDetailedMip = 0;
		desc.TextureCube.MipLevels = res->GetDesc().MipLevels;
		desc.TextureCube.ResourceMinLODClamp = 0.0f;

		hpManager.GetDev()->CreateShaderResourceView(res.Get(), &desc, hpAlloc->hCpu[0]);
	}

	void MkTex_Cube::CopySrcFillDstOneDepth(ID3D12Resource* dstRes, UINT depthOrArrayIdx, ID3D12Resource* srcRes, ID3D12Device4* dev, ID3D12GraphicsCommandList* cl)
	{
		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(srcRes,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_SOURCE));

		/*
		dst目标资源描述符和src资源完全一样，只不过DepthOrArraySize是6;
		意味着目标资源的子资源数量是src资源的6倍

		dstSubNum=6*srcSubNum；
		例如src子资源为 8个Mip
		dst的子资源为 6 *（8个Mip）
		*/
		D3D12_RESOURCE_DESC dstDesc = dstRes->GetDesc();
		int dstSubNum = dstDesc.MipLevels * dstDesc.DepthOrArraySize;

		D3D12_RESOURCE_DESC srcDesc = srcRes->GetDesc();
		int srcSubNum = srcDesc.MipLevels * srcDesc.DepthOrArraySize;

		/*CopySrcFillDstOneDepth函数仅仅对 第 depthOrArrayIdx 个的（8个Mip）进行复制*/
		for (UINT i = 0; i < srcSubNum; ++i)
		{
			CD3DX12_TEXTURE_COPY_LOCATION Dst(dstRes, i + depthOrArrayIdx * dstDesc.MipLevels);
			CD3DX12_TEXTURE_COPY_LOCATION Src(srcRes, i);
			cl->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
		}

		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(srcRes,
			D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		//ShowSubResFootPrintInfo(dstRes, comDev);
	}

	MkTex_Cube::MkTex_Cube(std::string name, ID3D12Resource* posx, ID3D12Resource* negx, ID3D12Resource* posy, ID3D12Resource* negy, ID3D12Resource* posz, ID3D12Resource* negz, ID3D12GraphicsCommandList* cl, HeapManager& hpManager) :name(std::move(name))
	{
		/*
		CubeMap是由6个图片构成的，此处直接赋值组第1个图片的资源描述，仅仅将DepthOrArraySize属性改为6
		意味是由6张完全同格式的资源组成
		*/
		D3D12_RESOURCE_DESC desc = posx->GetDesc();
		desc.DepthOrArraySize = 6;



		RWin32::ThrowIfFailed(hpManager.GetDev()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,// pOptimizedClearValue must be NULL when D3D12_RESOURCE_DESC::Dimension is not D3D12_RESOURCE_DIMENSION_BUFFER and neither D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET nor D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL are set in D3D12_RESOURCE_DESC::Flags. [ STATE_CREATION ERROR #815: CREATERESOURCE_INVALIDCLEARVALUE]
			IID_PPV_ARGS(&res)));

		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(res.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));

		CopySrcFillDstOneDepth(res.Get(), 0, posx, hpManager.GetDev(), cl);
		CopySrcFillDstOneDepth(res.Get(), 1, negx, hpManager.GetDev(), cl);
		CopySrcFillDstOneDepth(res.Get(), 2, posy, hpManager.GetDev(), cl);
		CopySrcFillDstOneDepth(res.Get(), 3, negy, hpManager.GetDev(), cl);
		CopySrcFillDstOneDepth(res.Get(), 4, posz, hpManager.GetDev(), cl);
		CopySrcFillDstOneDepth(res.Get(), 5, negz, hpManager.GetDev(), cl);

		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(res.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

		Bind(hpManager);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE MkTex_Cube::hCpu()
	{
		return hpAlloc->hCpu[0];
	}




	void RtvTex::Bind(HeapManager& hpManager)
	{

		if (rtvAlloc == nullptr) { rtvAlloc = hpManager.rtvHeap_Cpu.Get(1); }

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Format = res->GetDesc().Format;
		rtvDesc.Texture2D.MipSlice = 0;
		rtvDesc.Texture2D.PlaneSlice = 0;

		hpManager.GetDev()->CreateRenderTargetView(res.Get(), &rtvDesc, rtvAlloc->hCpu[0]);

		if (srvAlloc == nullptr) { srvAlloc = hpManager.csuHeap_Cpu.Get(1); }

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = res->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = res->GetDesc().MipLevels;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

		hpManager.GetDev()->CreateShaderResourceView(res.Get(), &srvDesc, srvAlloc->hCpu[0]);
	}

	/*创建自定义Rtv*/
	void RtvTex::Create(UINT width, UINT height, DXGI_FORMAT fmt, HeapManager& hpManager, std::vector<float> clearColor)
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = 0;
		desc.Width = width;
		desc.Height = height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = fmt;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		assert(clearColor.size() == 4);
		D3D12_CLEAR_VALUE optClear{ fmt,{clearColor[0],clearColor[1],clearColor[2],clearColor[3]} };

		RWin32::ThrowIfFailed(hpManager.GetDev()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			&optClear,
			IID_PPV_ARGS(&res)));

		Bind(hpManager);
	}

	/*以交换链后缓冲创建Rtv*/
	void RtvTex::Create(IDXGISwapChain* swch, UINT idx, HeapManager& hpManager)
	{
		RWin32::ThrowIfFailed(swch->GetBuffer(idx, IID_PPV_ARGS(&(res))));

		Bind(hpManager);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RtvTex::hCpu_Rtv()
	{
		return rtvAlloc->hCpu[0];
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RtvTex::hCpu_Srv()
	{
		return srvAlloc->hCpu[0];
	}




	void RtvTex_Cube::Bind(HeapManager& hpManager)
	{
		if (rtvAlloc == nullptr) { rtvAlloc = hpManager.rtvHeap_Cpu.Get(6); }

		// Create RTV to each cube face.
		for (int i = 0; i < 6; ++i)
		{
			D3D12_RENDER_TARGET_VIEW_DESC desc;
			desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
			desc.Format = res->GetDesc().Format;
			desc.Texture2DArray.MipSlice = 0;
			desc.Texture2DArray.PlaneSlice = 0;
			desc.Texture2DArray.FirstArraySlice = i;	// Render target to ith element.
			desc.Texture2DArray.ArraySize = 1;			// Only view one element of the array.

			hpManager.GetDev()->CreateRenderTargetView(res.Get(), &desc, rtvAlloc->hCpu[i]);
		}

		if (srvAlloc == nullptr) { srvAlloc = hpManager.csuHeap_Cpu.Get(1); }

		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Format = res->GetDesc().Format;
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MostDetailedMip = 0;
		desc.TextureCube.MipLevels = res->GetDesc().MipLevels;
		desc.TextureCube.ResourceMinLODClamp = 0.0f;

		hpManager.GetDev()->CreateShaderResourceView(res.Get(), &desc, srvAlloc->hCpu[0]);
	}

	void RtvTex_Cube::Create(UINT width, UINT height, DXGI_FORMAT fmt, HeapManager& hpManager)
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = 0;
		desc.Width = width;
		desc.Height = height;
		desc.DepthOrArraySize = 6;
		desc.MipLevels = 1;
		desc.Format = fmt;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE optClear{ fmt,{ 0.0f, 0.2f, 0.4f, 1.0f } };

		RWin32::ThrowIfFailed(hpManager.GetDev()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			&optClear,
			IID_PPV_ARGS(&res)));

		Bind(hpManager);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RtvTex_Cube::hCpu_Rtv(int num)
	{
		return rtvAlloc->hCpu[num];
	}

	D3D12_CPU_DESCRIPTOR_HANDLE RtvTex_Cube::hCpu_Srv()
	{
		return srvAlloc->hCpu[0];
	}

	void MipTex_Cube::Bind(HeapManager& hpManager)
	{
		if (hpAlloc == nullptr) { hpAlloc = hpManager.csuHeap_Cpu.Get(1); }

		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.Format = res->GetDesc().Format;
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MostDetailedMip = 0;
		desc.TextureCube.MipLevels = res->GetDesc().MipLevels;
		desc.TextureCube.ResourceMinLODClamp = 0.0f;

		hpManager.GetDev()->CreateShaderResourceView(res.Get(), &desc, hpAlloc->hCpu[0]);
	}

	void MipTex_Cube::CopySrcFillDstOneMip(ID3D12Resource* dstRes, UINT mipLevel, ID3D12Resource* srcRes, ID3D12GraphicsCommandList* cl)
	{
		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(srcRes,
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_SOURCE));

		D3D12_RESOURCE_DESC dstDesc = dstRes->GetDesc();
		int dstMaxMip = dstDesc.MipLevels;

		for (UINT i = 0; i < 6; ++i)
		{
			CD3DX12_TEXTURE_COPY_LOCATION Dst(dstRes, i * dstMaxMip + mipLevel);
			CD3DX12_TEXTURE_COPY_LOCATION Src(srcRes, i);
			cl->CopyTextureRegion(&Dst, 0, 0, 0, &Src, nullptr);
		}

		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(srcRes,
			D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_GENERIC_READ));
	}

	void MipTex_Cube::Create(std::vector<ID3D12Resource*> cubeMips, ID3D12GraphicsCommandList* cl, HeapManager& hpManager)
	{
		bool check = true;
		UINT dstMaxMip = cubeMips.size();
		for (int mipLevel = 0; mipLevel < dstMaxMip; mipLevel++)
		{
			auto rtvRes = cubeMips[mipLevel];
			if (
				rtvRes->GetDesc().MipLevels != 1
				|| rtvRes->GetDesc().DepthOrArraySize != 6
				|| rtvRes->GetDesc().Width != cubeMips[0]->GetDesc().Width >> mipLevel
				|| rtvRes->GetDesc().Height != cubeMips[0]->GetDesc().Height >> mipLevel
				)
			{
				check = false;
				break;
			}
		}
		if (cubeMips.size() == 0)
		{
			check = false;
		}
		assert(check == true);

		/*
		CubeMap是由6个图片构成的，此处直接赋值组第1个图片的资源描述，仅仅将DepthOrArraySize属性改为6
		意味是由6张完全同格式的资源组成
		*/
		D3D12_RESOURCE_DESC desc = cubeMips[0]->GetDesc();
		desc.MipLevels = dstMaxMip;

		RWin32::ThrowIfFailed(hpManager.GetDev()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,// pOptimizedClearValue must be NULL when D3D12_RESOURCE_DESC::Dimension is not D3D12_RESOURCE_DIMENSION_BUFFER and neither D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET nor D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL are set in D3D12_RESOURCE_DESC::Flags. [ STATE_CREATION ERROR #815: CREATERESOURCE_INVALIDCLEARVALUE]
			IID_PPV_ARGS(&res)));

		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(res.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COPY_DEST));
		
		for (int mipLevel = 0; mipLevel < dstMaxMip; mipLevel++)
		{	
			CopySrcFillDstOneMip(res.Get(), mipLevel, cubeMips[mipLevel], cl);
		}

		cl->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(res.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

		Bind(hpManager);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE MipTex_Cube::hCpu()
	{
		return hpAlloc->hCpu[0];
	}


	void DsvTex::Bind(HeapManager& hpManager)
	{
		if (dsvAlloc == nullptr) { dsvAlloc = hpManager.dsvHeap_Cpu.Get(1); }
		D3D12_DEPTH_STENCIL_VIEW_DESC descDsv = {};
		descDsv.Flags = D3D12_DSV_FLAG_NONE;
		descDsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		descDsv.Format = dsvFmt;
		descDsv.Texture2D.MipSlice = 0;
		hpManager.GetDev()->CreateDepthStencilView(res.Get(), &descDsv, dsvAlloc->hCpu[0]);

		if (srvAlloc == nullptr) { srvAlloc = hpManager.csuHeap_Cpu.Get(1); }
		D3D12_SHADER_RESOURCE_VIEW_DESC descSrv = {};
		descSrv.Texture2D.MipLevels = res.Get()->GetDesc().MipLevels;
		descSrv.Texture2D.MostDetailedMip = 0;
		descSrv.Format = srvFmt;
		descSrv.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		descSrv.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		hpManager.GetDev()->CreateShaderResourceView(res.Get(), &descSrv, srvAlloc->hCpu[0]);
	}

	void DsvTex::Create(UINT width, UINT height, HeapManager& hpManager)
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = 0;
		desc.Width = width;
		desc.Height = height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
		// the depth buffer.  Therefore, because we need to create two views to the same resource:
		//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
		//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
		// we need to create the depth buffer resource with a typeless format.  
		desc.Format = resFmt;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optClear = {};
		optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		optClear.DepthStencil.Depth = 1.0f;
		optClear.DepthStencil.Stencil = 0;

		RWin32::ThrowIfFailed(hpManager.GetDev()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optClear,
			IID_PPV_ARGS(&res)));

		Bind(hpManager);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DsvTex::hCpu_Dsv()
	{
		return dsvAlloc->hCpu[0];
	}

	D3D12_CPU_DESCRIPTOR_HANDLE DsvTex::hCpu_Srv()
	{
		return srvAlloc->hCpu[0];
	}

	void ComputeTex::Bind(HeapManager& hpManager)
	{
		if (csuAlloc == nullptr) { csuAlloc = hpManager.csuHeap_Cpu.Get(2); }
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = res->GetDesc().Format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		hpManager.GetDev()->CreateShaderResourceView(res.Get(), &srvDesc, csuAlloc->hCpu[0]);

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = res->GetDesc().Format;
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		hpManager.GetDev()->CreateUnorderedAccessView(res.Get(), nullptr, &uavDesc, csuAlloc->hCpu[1]);
	}

	void ComputeTex::Create(UINT width, UINT height, DXGI_FORMAT fmt, HeapManager& hpManager)
	{
		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = 0;
		desc.Width = width;
		desc.Height = height;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = fmt;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

		RWin32::ThrowIfFailed(hpManager.GetDev()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(&res)));

		Bind(hpManager);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE ComputeTex::hCpu_Srv()
	{
		return csuAlloc->hCpu[0];
	}

	D3D12_CPU_DESCRIPTOR_HANDLE ComputeTex::hCpu_Uav()
	{
		return csuAlloc->hCpu[1];
	}


}