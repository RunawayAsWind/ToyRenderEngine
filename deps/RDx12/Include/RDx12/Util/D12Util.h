#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <RWin32/Win32Debug.h>

namespace RDx12
{
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBufferByData(
		ID3D12Device4* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

	void UploadToDefaultTexByData(
		ID3D12Device4* dev,
		ID3D12GraphicsCommandList* cmdList,
		Microsoft::WRL::ComPtr<ID3D12Resource>& res,		//传入的res必须已创建，并已经从其他格式纹理中解码获取尺寸大小等必要信息，方便用res->GetDesc()来获取相关数据，为填充做准备
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadRes,
		D3D12_SUBRESOURCE_DATA& subresource);

	UINT CalcConstantBufferByteSize(UINT byteSize);

	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootSign(
		ID3D12Device4* device,
		const D3D12_ROOT_SIGNATURE_DESC desc);

	Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target);

	void ShowSubResFootPrintInfo(ID3D12Resource* res, ID3D12Device4* dev);
	void ShowResInfo(ID3D12Resource* res);

}