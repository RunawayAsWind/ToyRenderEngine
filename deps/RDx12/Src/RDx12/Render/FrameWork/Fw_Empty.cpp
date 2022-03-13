#include "../Include/RDx12/Render/FrameWork/Fw_Empty.h"
#include <RWin32/Win32Debug.h>

namespace RDx12
{
	Fw_Empty::Fw_Empty(HINSTANCE hInst, HWND hWnd):
		m_Hinst(hInst), m_Hwnd(hWnd), inputDetect{ hInst,hWnd },
		hpManager{ dx12Base.GetDevice() }
	{
		DXGI_SWAP_CHAIN_DESC1 desc = baseSwch.DefaultDesc(2, DXGI_FORMAT_R8G8B8A8_UNORM, 1, dx12Base.GetDevice());
		baseSwch.CreateSwapChain(desc,dx12Base.GetFactory(), dx12Base.baseCmdQue.GetCmdQue(), m_Hwnd);
		baseSwch.NoAltEnter(dx12Base.GetFactory(), m_Hwnd);
		gameTimer.Reset();
		rtv_BackBuffer.resize(baseSwch.GetDesc().BufferCount);

		imMain.Create(hWnd, dx12Base.GetDevice(), baseSwch.GetDesc().BufferCount + 1, baseSwch.GetDesc().Format);
		//SetSizeAndCreateView(baseSwch.GetDesc().Width, baseSwch.GetDesc().Height);//��Ҫ�ڹ��캯���е����麯�����ڹ��캯���е����麯���޷������������ʵ��
	}

	void Fw_Empty::SetSizeAndCreateViewByhWndSize()
	{
		RECT rc;

		GetClientRect(m_Hwnd, &rc);
		int width = rc.right - rc.left;   //���ڵĿ��
		int height = rc.bottom - rc.top;  //���ڵĸ߶�
		if (width != baseSwch.GetDesc().Width || height != baseSwch.GetDesc().Height)
		{
			WIN32DEBUG(L"width:%d height:%d\n", width, height);
			SetSizeAndCreateView(width, height);
			//cam.SetFrustum(1.0f, 1000.0f, static_cast<float>(width) / height, 90);
		}
	}

	void Fw_Empty::SetSizeAndCreateView(UINT newW, UINT newH)
	{
		//ִ����֮ǰ��ָ��
		dx12Base.baseCmdQue.FlushCmdQue();
		dx12Base.baseCmdQue.ListReset();

		//����Ҫ���ͷ������Դ�����ܹ�����ResizeBuffers(),����ᱨ��
		for (UINT i = 0; i < baseSwch.GetDesc().BufferCount; i++)
		{
			rtv_BackBuffer[i].res.Reset();
		}
		rtv_BackDsv.res.Reset();

		baseSwch.Resize(newW, newH);
		
		for (UINT i = 0; i < baseSwch.GetDesc().BufferCount; i++)
		{
			rtv_BackBuffer[i].Create(baseSwch.GetSch().Get(), i, hpManager);
		}
		rtv_BackDsv.Create(baseSwch.GetDesc().Width, baseSwch.GetDesc().Height, hpManager);
		
		imMain.Resize();
		ReSizeOtherRes();//ע�⣬�������麯�����÷��������ڹ��캯���е���

		//ִ����Դת��ָ��
		dx12Base.baseCmdQue.CloseAndExecute();
		//�ȴ�ָ�����
		dx12Base.baseCmdQue.FlushCmdQue();


		m_ViewPort.TopLeftX = 0;
		m_ViewPort.TopLeftY = 0;
		m_ViewPort.Width = static_cast<float>(newW);
		m_ViewPort.Height = static_cast<float>(newH);
		m_ViewPort.MinDepth = 0.0f;
		m_ViewPort.MaxDepth = 1.0f;

		m_ScissorRect.left = 0;
		m_ScissorRect.top = 0;
		m_ScissorRect.right = static_cast<long>(newW);
		m_ScissorRect.bottom = static_cast<long>(newH);

	}

	void Fw_Empty::ReSizeOtherRes()
	{

	}

	void Fw_Empty::InitRes()
	{
		SetSizeAndCreateView(baseSwch.GetDesc().Width, baseSwch.GetDesc().Height);
	}

	void Fw_Empty::MainLoop()
	{
		while (!b_Quit)
		{
			gameTimer.Tick();
			//WIN32DEBUG(L"fps: %d\n", (int)(1.0f / gameTimer.DeltaTime()));
			SetSizeAndCreateViewByhWndSize();

			if (!b_Stop)
			{
				inputDetect.GetInput(gameTimer.DeltaTime());
				InputHandle(gameTimer.DeltaTime());
				gameTimer.GetCostTime("Update", [this]() {Update(gameTimer.DeltaTime()); });
				gameTimer.GetCostTime("Render", [this]() {Render(); });
			}
			//hpManager.FreeBlock();
		}
		//imMain.Release();
		b_CanQuit = true;
	}

	void Fw_Empty::Update(float deltatime)
	{
		//UpdateSceneNode(deltatime);

		//framevec.Update(cCmdQueue.compFence.Get());
		//UpdateObjectCBs(deltatime);
		//UpdateMainPassCB(deltatime);
		//WIN32DEBUG(L"NowTime: %f\n", gameTimer.TotalTime());
	}

	void Fw_Empty::InputHandle(float deltatime)
	{
		gameTimer.Tick();
		if (inputDetect.KeyDown(RDx12::InputKey::IK_W))
		{
			//cam.SetPos(XMFLOAT3(cam.m_Pos.x, cam.m_Pos.y, cam.m_Pos.z + deltatime));
			gameTimer.Tick();
			WIN32DEBUG(L"%f \n", gameTimer.DeltaTime());
		}
		if (inputDetect.KeyDown(RDx12::InputKey::IK_S))
		{
			//cam.SetPos(XMFLOAT3(cam.m_Pos.x, cam.m_Pos.y, cam.m_Pos.z - deltatime));
		}
		if (inputDetect.KeyDown(RDx12::InputKey::IK_A))
		{
			//cam.SetPos(XMFLOAT3(cam.m_Pos.x + deltatime, cam.m_Pos.y, cam.m_Pos.z));
		}
		if (inputDetect.KeyDown(RDx12::InputKey::IK_D))
		{
			//cam.SetPos(XMFLOAT3(cam.m_Pos.x - deltatime, cam.m_Pos.y, cam.m_Pos.z));
		}
		//WJAPI_Win32_DebugOut(TEXT("cam-z:%f\n"), cam.m_Pos.z);
		//cam.UpdateViewMat();
	}

	void  Fw_Empty::Render()
	{

	}

	Fw_Empty::~Fw_Empty()
	{
	}

	void Fw_Empty::StartRender()
	{
		b_Stop = false;
		b_Quit = false;
		b_CanQuit = false;
		MainLoop();
	}

	void Fw_Empty::ResumRender()
	{
		b_Stop = false;
	}

	void Fw_Empty::StopRender()
	{
		b_Stop = true;
	}

	void Fw_Empty::QuitRender()
	{
		b_Stop = true;
		b_Quit = true;
		while (!b_CanQuit)
		{
		}
	}
}