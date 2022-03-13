#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <RWin32/Win32Debug.h>

namespace RDx12
{
    //单位帧资源，帧资源的主要目的是在上一帧执行GPU指令的时候，提前开始下一帧的CPU指令执行,从而提高两者的工作占有率
    class FrBase
    {
    public:
        /*
        每帧具有独立的CommandAllocator和fenceNum
        fenceNum用于记录该帧GPU指令是否执行结束（详情可见Fw_Fr的Render()）
        */
        FrBase(ID3D12Device4* device)
        { RWin32::ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdListAlloc))); }
        
        inline void SetFenceNum(UINT newNum)
        { fenceNum = newNum; }
        
        inline UINT64 GetFenceNum() const noexcept
        { return fenceNum; }
        
        inline ID3D12CommandAllocator* GetListAlloc() const noexcept
        { return cmdListAlloc.Get(); }

    private:
        Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdListAlloc;
        UINT64 fenceNum = 0;
    };
}