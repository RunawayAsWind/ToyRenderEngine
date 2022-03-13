#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <RWin32/Win32Debug.h>

namespace RDx12
{
    //��λ֡��Դ��֡��Դ����ҪĿ��������һִ֡��GPUָ���ʱ����ǰ��ʼ��һ֡��CPUָ��ִ��,�Ӷ�������ߵĹ���ռ����
    class FrBase
    {
    public:
        /*
        ÿ֡���ж�����CommandAllocator��fenceNum
        fenceNum���ڼ�¼��֡GPUָ���Ƿ�ִ�н���������ɼ�Fw_Fr��Render()��
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