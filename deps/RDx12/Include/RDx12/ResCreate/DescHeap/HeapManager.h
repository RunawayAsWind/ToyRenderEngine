#pragma once
#include "DescHeapPool.h"

namespace RDx12 {
    class HeapManager
    {
    public:
        //通过调用以下描述符池的Get函数获得
        DescHeapPool dsvHeap_Cpu;
        DescHeapPool rtvHeap_Cpu;
        DescHeapPool csuHeap_Cpu;
        DescHeapPool smpHeap_Cpu;
        DescHeapPool csuHeap_Gpu;
        DescHeapPool smpHeap_Gpu;

        HeapManager(Microsoft::WRL::ComPtr<ID3D12Device4> dev) :
            dev(dev),
            dsvHeap_Cpu("dsvHeap_Cpu",0, { D3D12_DESCRIPTOR_HEAP_TYPE_DSV,100,D3D12_DESCRIPTOR_HEAP_FLAG_NONE ,0 }, dev),
            rtvHeap_Cpu("rtvHeap_Cpu",0, { D3D12_DESCRIPTOR_HEAP_TYPE_RTV,100,D3D12_DESCRIPTOR_HEAP_FLAG_NONE ,0 }, dev),

            //因为部分Shader中连续的数据，例如PBR的十张纹理贴图需要尽量一次性setTable进行设置，所以各Srv资源拥有一个单独的Cpu Descriptor，然后按一定次序复制到GpuHeap中
            csuHeap_Cpu("csuHeap_Cpu",0, { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,100,D3D12_DESCRIPTOR_HEAP_FLAG_NONE ,0 }, dev),
            smpHeap_Cpu("smpHeap_Cpu",0, { D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,100,D3D12_DESCRIPTOR_HEAP_FLAG_NONE ,0 }, dev),
            
            //将上面两个分发的Cpu Descriptor复制到下面两个堆中，再Render循环中只需要通过SetDescriptorHeaps设置下面两个Gpu堆
            //注意因为cl->SetDescriptorHeaps最多同时设置一个CSU和一个SAMPLER Heap，所以这两个Heap池一般只使用第一个，且第一个要足够大
            csuHeap_Gpu("csuHeap_Gpu",1, { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,500,D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE ,0 }, dev),
            smpHeap_Gpu("smpHeap_Gpu",1, { D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,500,D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE ,0 }, dev)
        {}

        //在每帧末尾调用回收掉所有资源
        void FreeBlock()
        {
            dsvHeap_Cpu.FreeBlock();
            rtvHeap_Cpu.FreeBlock();
            csuHeap_Cpu.FreeBlock();
            smpHeap_Cpu.FreeBlock();
            csuHeap_Gpu.FreeBlock();
            smpHeap_Gpu.FreeBlock();
        }

        ID3D12Device4* GetDev()
        {
            return dev.Get();
        }
    private:
        Microsoft::WRL::ComPtr<ID3D12Device4> dev;
    };
}