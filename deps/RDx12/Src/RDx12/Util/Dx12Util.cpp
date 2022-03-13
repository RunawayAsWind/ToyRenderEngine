#include "../../../Include/RDx12/Util/D12Util.h"
#include "../../../3rdParty/Dx12Dragon/d3dx12.h"

namespace RDx12
{
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBufferByData(
		ID3D12Device4* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> defaultBuffer;

		// Create the actual default buffer resource.
		RWin32::ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

		// In order to copy CPU memory data into our default buffer, we need to create
		// an intermediate upload heap. 
		RWin32::ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.GetAddressOf())));


		// Describe the data we want to copy into the default buffer.
		D3D12_SUBRESOURCE_DATA subResourceData = {};
		subResourceData.pData = initData;
		subResourceData.RowPitch = byteSize;
		subResourceData.SlicePitch = subResourceData.RowPitch;

		// Schedule to copy the data to the default buffer resource.  At a high level, the helper function UpdateSubresources
		// will copy the CPU memory into the intermediate upload heap.  Then, using ID3D12CommandList::CopySubresourceRegion,
		// the intermediate upload heap data will be copied to mBuffer.
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));
		UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);
		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ));

		// Note: uploadBuffer has to be kept alive after the above function calls because
		// the command list has not been executed yet that performs the actual copy.
		// The caller can Release the uploadBuffer after it knows the copy has been executed.


		return defaultBuffer;
	}

	void UploadToDefaultTexByData(
		ID3D12Device4* dev,
		ID3D12GraphicsCommandList* cmdList,
		Microsoft::WRL::ComPtr<ID3D12Resource>& res,		//传入的res必须已创建，并已经从其他格式纹理中解码获取尺寸大小等必要信息，方便用res->GetDesc()来获取相关数据，为填充做准备
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadRes,
		D3D12_SUBRESOURCE_DATA& subresource)
	{
		UINT num2DSubresources = res->GetDesc().DepthOrArraySize * res->GetDesc().MipLevels;
		UINT64 uploadBufferSize = GetRequiredIntermediateSize(res.Get(), 0, num2DSubresources);

		auto hr = dev->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadRes));
		if (FAILED(hr))
		{
			uploadRes = nullptr;
		}
		else
		{
			// Use Heap-allocating UpdateSubresources implementation for variable number of subresources (which is the case for textures).
			UpdateSubresources(cmdList, res.Get(), uploadRes.Get(), 0, 0, num2DSubresources, &subresource);

			cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(res.Get(),
				D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));
		}
	}

	UINT CalcConstantBufferByteSize(UINT byteSize)
	{
		// Constant buffers must be a multiple of the minimum hardware
		// allocation size (usually 256 bytes).  So round up to nearest
		// multiple of 256.  We do this by adding 255 and then masking off
		// the lower 2 bytes which store all bits < 256.
		// Example: Suppose byteSize = 300.
		// (300 + 255) & ~255
		// 555 & ~255
		// 0x022B & ~0x00ff
		// 0x022B & 0xff00
		// 0x0200
		// 512
		return (byteSize + 255) & ~255;
	}

	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootSign(
		ID3D12Device4* device,
		const D3D12_ROOT_SIGNATURE_DESC desc)
	{
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootsign;

		HRESULT hr = S_OK;

		Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errors = nullptr;
		hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1,&serializedRootSig, &errors);

		if (errors != nullptr)
			OutputDebugStringA((char*)errors->GetBufferPointer());

		RWin32::ThrowIfFailed(hr);

		RWin32::ThrowIfFailed(device->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(&rootsign)));

		return rootsign;
	}

	Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target)
	{
		UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		HRESULT hr = S_OK;

		Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errors = nullptr;
		hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

		if (errors != nullptr)
			OutputDebugStringA((char*)errors->GetBufferPointer());

		RWin32::ThrowIfFailed(hr);

		return byteCode;
	}

	void ShowSubResFootPrintInfo(ID3D12Resource* res, ID3D12Device4* dev)
	{
		D3D12_RESOURCE_DESC desc = res->GetDesc();
		
		int MaxSubIndex = desc.MipLevels * desc.DepthOrArraySize;
		UINT64 RequiredSize = 0;
		std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> Layouts(MaxSubIndex);
		std::vector<UINT> NumRows(MaxSubIndex);
		std::vector<UINT64> RowSizesInBytes(MaxSubIndex);

		dev->GetCopyableFootprints(&desc, 0, MaxSubIndex, 0, Layouts.data(), NumRows.data(), RowSizesInBytes.data(), &RequiredSize);

		WIN32DEBUG(L"\n");
		WIN32DEBUG(L"--------- Dx12Util.h SubRes_FootPrint: %d -----------\n", MaxSubIndex);
		for (int i = 0; i < MaxSubIndex; i++)
		{
			WIN32DEBUG(L"Idx: %d\n", i);
			WIN32DEBUG(L"Offset: %d\n", Layouts[i].Offset);
			WIN32DEBUG(L"FootPrint Width: %d  Height: %d  Depth: %d \n", Layouts[i].Footprint.Width, Layouts[i].Footprint.Height, Layouts[i].Footprint.Depth);
			WIN32DEBUG(L"RowPitch: %d  Format: %d\n", Layouts[i].Footprint.RowPitch, Layouts[i].Footprint.Format);
			WIN32DEBUG(L"NumRows: %d  RowSizesInBytes: %d\n", NumRows[i], RowSizesInBytes[i]);
			WIN32DEBUG(L"\n");
		}
		WIN32DEBUG(L"RequiredSize: %d\n", RequiredSize);
		WIN32DEBUG(L"\n");
	}

	void ShowResInfo(ID3D12Resource* res)
	{
		D3D12_RESOURCE_DESC desc = res->GetDesc();
		WIN32DEBUG(L"\n");
		WIN32DEBUG(L"------- Dx12Util.h Res_Desc_Info --------\n");
		WIN32DEBUG(L"Width: %d Height: %d\n",desc.Width,desc.Height);
		WIN32DEBUG(L"Array/Depth: %d Mip:%d Align:%d\n", desc.DepthOrArraySize, desc.MipLevels,desc.Alignment);
		WIN32DEBUG(L"Dim: %d Fmt: %d Flag: %d Layout: %d\n", desc.Dimension, desc.Format, desc.Flags, desc.Layout);
		WIN32DEBUG(L"SampleCount: %d SampleQuailty: %d Flag: %d\n", desc.SampleDesc.Count, desc.SampleDesc.Quality);
		WIN32DEBUG(L"\n");
	}
}