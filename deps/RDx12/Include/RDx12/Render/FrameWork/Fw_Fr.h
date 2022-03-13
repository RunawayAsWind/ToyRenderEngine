#pragma once
#include "Fw_Empty.h"
#include "../../FrameResource/FrVec.h"
#include "../../FrameResource/FrBase.h"

namespace RDx12
{
	class Fw_Fr :public Fw_Empty
	{
		
	protected:
		Fw_Fr(HINSTANCE hInst, HWND hWnd) :Fw_Empty{ hInst, hWnd } {};
		//----------InitRes------------
		virtual void InitDetailRes();

		//----------MainLoop------------
		virtual void ReSizeOtherRes() override;
		virtual void InputHandle(float deltatime) override;
		virtual void Update(float deltatime) override;
		virtual void Render() override final;
		virtual void RenderLogic(ID3D12GraphicsCommandList* cl);
	public:
	
		FrVec frvec;
		virtual void InitRes() override final;
	};
}