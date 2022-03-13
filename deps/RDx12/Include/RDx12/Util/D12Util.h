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
		Microsoft::WRL::ComPtr<ID3D12Resource>& res,		//�����res�����Ѵ��������Ѿ���������ʽ�����н����ȡ�ߴ��С�ȱ�Ҫ��Ϣ��������res->GetDesc()����ȡ������ݣ�Ϊ�����׼��
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