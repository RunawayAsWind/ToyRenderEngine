#include "../Include/RDx12/D12Basis/D12CmdQue.h"

#include <RWin32/Win32Debug.h>

namespace RDx12
{
	D12CmdQue::D12CmdQue(ID3D12Device4* dev)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		RWin32::ThrowIfFailed(dev->CreateCommandQueue(&desc, IID_PPV_ARGS(&cmdQue)),L"D12CmdQue:cmdQue创建成功\n",L"D12CmdQue:cmdQue创建失败！！！！\n");

		RWin32::ThrowIfFailed(dev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&cmdFence)), L"D12CmdQue:cmdFence创建成功\n", L"D12CmdQue:cmdFence创建失败！！！！\n");
		RWin32::ThrowIfFailed(dev->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(&cmdAlloc)), L"D12CmdQue:cmdAlloc创建成功\n", L"D12CmdQue:cmdAlloc创建失败！！！！\n");

		//由于我们当前把pso置空了，当我们开始渲染管线的时候需要通过调用Reset函数来设置Comlist的pso，再调用Reset前需要把comlist关闭
		//                                                                                    | pso=nullptr |
		RWin32::ThrowIfFailed(dev->CreateCommandList(0, desc.Type, cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&cmdList)), L"D12CmdQue:cmdList创建成功\n", L"D12CmdQue:cmdList创建失败！！！！\n");
		RWin32::ThrowIfFailed(cmdList->Close());
	}

	void D12CmdQue::ListReset()
	{
		RWin32::ThrowIfFailed(cmdAlloc->Reset());//必须要加入这句，否则会cmdList执行后消耗的内存不能释放
		RWin32::ThrowIfFailed(cmdList->Reset(cmdAlloc.Get(), nullptr));
	}

	void D12CmdQue::CloseAndExecute()
	{
		RWin32::ThrowIfFailed(cmdList->Close());
		ID3D12CommandList* cmdsLists[] = { cmdList.Get() };
		cmdQue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	}

	void D12CmdQue::FlushCmdQue()
	{
		UpdateMainFeNum();
		SetCmdOverFeNum(mainFeNum);
		WaitFenceToFeNum(mainFeNum);
	}

	UINT D12CmdQue::UpdateMainFeNum()
	{
		mainFeNum++;
		return mainFeNum;
	}

	UINT D12CmdQue::GetMainFeNum()
	{
		return mainFeNum;
	}

	void D12CmdQue::SetCmdOverFeNum(UINT64 feNum)
	{
		RWin32::ThrowIfFailed(cmdQue->Signal(cmdFence.Get(), feNum));
	}

	void D12CmdQue::WaitFenceToFeNum(UINT64 feNum)
	{
		if (cmdFence->GetCompletedValue() < feNum)
		{
			HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
			RWin32::ThrowIfFailed(cmdFence->SetEventOnCompletion(feNum, eventHandle));
			WaitForSingleObject(eventHandle, INFINITE);
			CloseHandle(eventHandle);
		}
	}

	ID3D12CommandQueue* D12CmdQue::GetCmdQue()
	{
		return  cmdQue.Get();
	}

	ID3D12GraphicsCommandList* D12CmdQue::GetCmdList()
	{
		return cmdList.Get();
	}

	ID3D12Fence* D12CmdQue::GetCmdFence()
	{
		return cmdFence.Get();
	}

	
}