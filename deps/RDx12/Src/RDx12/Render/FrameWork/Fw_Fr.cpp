#include "../Include/RDx12/Render/FrameWork/Fw_Fr.h"
#include <RWin32/Win32Debug.h>

namespace RDx12
{	
	void  Fw_Fr::InitRes()
	{
		SetSizeAndCreateView(baseSwch.GetDesc().Width, baseSwch.GetDesc().Height);//�ڷǹ��캯��
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
		
		//�ύ����,�󻺳���Ÿ���
		RWin32::ThrowIfFailed(baseSwch.GetSch()->Present(1, 0));
		baseSwch.UpdateFrameIndex();
		
		//����֡���
		int mainFenceNum = dx12Base.baseCmdQue.UpdateMainFeNum();
		frvec.GetNowFr()->SetFenceNum(mainFenceNum);

		//����֡�ύ������ָ�����ʱ��Que��Ż���ڵ��ڸ�֡���
		dx12Base.baseCmdQue.SetCmdOverFeNum(mainFenceNum);

		//���µ���֡
		int fridx = frvec.UpdateFrResIdx();
	    //frvec.ShowFrFeNumInfo();
		//WIN32DEBUG(L"nowfridx: %d  cmdfence:%d  <  frfence: %d  ?   needwait: %d\n", fridx, dx12Base.baseCmdQue.GetCmdFence()->GetCompletedValue(), frvec.GetNowFr()->GetFenceNum(), dx12Base.baseCmdQue.GetCmdFence()->GetCompletedValue() < frvec.GetNowFr()->GetFenceNum());
		dx12Base.baseCmdQue.WaitFenceToFeNum(frvec.GetNowFr()->GetFenceNum()); //�ڲ��Ƚ�Que��ź���֡��ţ����Que��Žϴ�˵���ϸ����ڵ���ָ֡��ִ���꣬����ֱ�ӿ�ʼ��֡��Cpuָ��
	}

	void  Fw_Fr::RenderLogic(ID3D12GraphicsCommandList* cl)
	{

	}
}