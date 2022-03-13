#pragma once
#include <d3d12.h>
#include <wrl.h>

namespace RDx12
{

	//������п��Կ���DX10��DX11�е�DeviceContext���ܲ�ֿ���,������Ϣ������ʽ����
	//https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_command_queue_desc
	class D12CmdQue
	{
	public:
		D12CmdQue(ID3D12Device4* dev);

		//����cmdlist��Ҫִ�е�ʱ��һ�·�ʽ����
		/*
		FlushCmdQue();
		void ListReset();

		//�˴�дҪ�õ�cmdlist�Ĵ���

		void CloseAndExecute();
		FlushCmdQue();
		*/
		void ListReset();
		void CloseAndExecute();
		void FlushCmdQue();
		
		UINT UpdateMainFeNum();
		UINT GetMainFeNum();
		void SetCmdOverFeNum(UINT64 feNum);
		void WaitFenceToFeNum(UINT64 feNum);

		ID3D12CommandQueue* GetCmdQue();
		ID3D12GraphicsCommandList* GetCmdList();
		ID3D12Fence* GetCmdFence();
		
	private:
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> cmdQue;
		
		/*
		ע�������֮Ϊ��cmlist����cmlistһ�㲻����֡��Ⱦ��һ��������Ⱦǰ��Դ��������޸���Դ���ԡ�
		������ЩResource�ڴ����׶���Ҫ����cmdList->ResourceBarrier
		*/
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAlloc;
		Microsoft::WRL::ComPtr<ID3D12Fence> cmdFence;
		UINT mainFeNum = 0;

	};
}