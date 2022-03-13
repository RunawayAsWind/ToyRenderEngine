#pragma once
#include <RWin32/GameTimer.h>
#include <RImgui/ImMain.h>
#include <vector>
#include "../../D12Basis/D12Basis.h"
#include "../../D12Basis/D12SwapChain.h"
#include "../../UserInput/InputDetect.h"
#include "../../ResCreate/DescHeap/HeapManager.h"
#include "../../ResCreate/Res/Texture/Texture.h"


/*
最基础的渲染框架，只实现了自适应窗口大小
*/

namespace RDx12
{
	class Fw_Empty
	{
	protected:
		HINSTANCE m_Hinst;
		HWND m_Hwnd;
		D12Basis dx12Base;
		HeapManager hpManager;
		D12SwapChain baseSwch;

		RWin32::GameTimer gameTimer;
		CD3DX12_VIEWPORT m_ViewPort;
		CD3DX12_RECT	 m_ScissorRect;
		bool b_Quit = false;
		bool b_CanQuit = false;
		bool b_Stop = false;

		//BaseRes
		std::vector<RtvTex> rtv_BackBuffer;
		DsvTex rtv_BackDsv;

		//----------MainLoop------------
		void SetSizeAndCreateViewByhWndSize();
		void SetSizeAndCreateView(UINT newW, UINT newH);
			virtual void ReSizeOtherRes();

		virtual void InputHandle(float deltatime);
		virtual void Update(float deltatime);
		virtual void Render();

	public:
		//ImGui
		RImgui::ImMain imMain;
		InputDetect inputDetect;
		Fw_Empty(HINSTANCE hInst,HWND hWnd);
		virtual void InitRes();
		void MainLoop();
		void StartRender();
		void ResumRender();
		void StopRender();
		void QuitRender();
		virtual ~Fw_Empty();

	};
}