#pragma once
#include <d3d12.h>
#include <wrl.h>

namespace RDx12
{

	//命令队列可以堪称DX10和DX11中的DeviceContext功能拆分开来,并以消息队列形式操作
	//https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_command_queue_desc
	class D12CmdQue
	{
	public:
		D12CmdQue(ID3D12Device4* dev);

		//在有cmdlist需要执行得时候按一下方式调用
		/*
		FlushCmdQue();
		void ListReset();

		//此处写要用到cmdlist的代码

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
		注意下面称之为主cmlist，主cmlist一般不用于帧渲染，一般用于渲染前资源处理或者修改资源属性。
		例如有些Resource在创建阶段需要调用cmdList->ResourceBarrier
		*/
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAlloc;
		Microsoft::WRL::ComPtr<ID3D12Fence> cmdFence;
		UINT mainFeNum = 0;

	};
}