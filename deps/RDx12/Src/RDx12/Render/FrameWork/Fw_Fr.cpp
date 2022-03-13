#include "../Include/RDx12/Render/FrameWork/Fw_Fr.h"
#include <RWin32/Win32Debug.h>

namespace RDx12
{	
	void  Fw_Fr::InitRes()
	{
		SetSizeAndCreateView(baseSwch.GetDesc().Width, baseSwch.GetDesc().Height);//在非构造函数
		frvec.Create(dx12Base.GetDevice(), baseSwch.GetDesc().BufferCount+1);
		InitDetailRes();
	}

	void Fw_Fr::InitDetailRes()
	{

	}

	void Fw_Fr::ReSizeOtherRes()
	{

	}

	void Fw_Fr::InputHandle(float deltatime)
	{

	}

	void Fw_Fr::Update(float deltatime)
	{
		/*

		UpdateSceneNode(deltatime);
		UpdateObjectCBs(deltatime);
		UpdateMainPassCB(deltatime);

		*/
	}

	void  Fw_Fr::Render()
	{
		ID3D12CommandAllocator* ca = frvec.GetNowFr()->GetListAlloc();
		ID3D12GraphicsCommandList* cl = dx12Base.baseCmdQue.GetCmdList();

		RWin32::ThrowIfFailed(ca->Reset());
		RWin32::ThrowIfFailed(cl->Reset(ca, nullptr));

		RenderLogic(cl);

		dx12Base.baseCmdQue.CloseAndExecute();
		
		//提交画面,后缓冲序号更新
		RWin32::ThrowIfFailed(baseSwch.GetSch()->Present(1, 0));
		baseSwch.UpdateFrameIndex();
		
		//赋予帧序号
		int mainFenceNum = dx12Base.baseCmdQue.UpdateMainFeNum();
		frvec.GetNowFr()->SetFenceNum(mainFenceNum);

		//当该帧提交的所有指令完成时，Que序号会大于等于该帧序号
		dx12Base.baseCmdQue.SetCmdOverFeNum(mainFenceNum);

		//更新到下帧
		int fridx = frvec.UpdateFrResIdx();
	    //frvec.ShowFrFeNumInfo();
		//WIN32DEBUG(L"nowfridx: %d  cmdfence:%d  <  frfence: %d  ?   needwait: %d\n", fridx, dx12Base.baseCmdQue.GetCmdFence()->GetCompletedValue(), frvec.GetNowFr()->GetFenceNum(), dx12Base.baseCmdQue.GetCmdFence()->GetCompletedValue() < frvec.GetNowFr()->GetFenceNum());
		dx12Base.baseCmdQue.WaitFenceToFeNum(frvec.GetNowFr()->GetFenceNum()); //内部比较Que序号和下帧序号，如果Que序号较大，说明上个周期的下帧指令执行完，可以直接开始下帧的Cpu指令
	}

	void  Fw_Fr::RenderLogic(ID3D12GraphicsCommandList* cl)
	{

	}
}