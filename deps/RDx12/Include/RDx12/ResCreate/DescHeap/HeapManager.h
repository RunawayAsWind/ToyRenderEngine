#pragma once
#include "DescHeapPool.h"

namespace RDx12 {
    class HeapManager
    {
    public:
        //ͨ�����������������ص�Get�������
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

            //��Ϊ����Shader�����������ݣ�����PBR��ʮ��������ͼ��Ҫ����һ����setTable�������ã����Ը�Srv��Դӵ��һ��������Cpu Descriptor��Ȼ��һ�������Ƶ�GpuHeap��
            csuHeap_Cpu("csuHeap_Cpu",0, { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,100,D3D12_DESCRIPTOR_HEAP_FLAG_NONE ,0 }, dev),
            smpHeap_Cpu("smpHeap_Cpu",0, { D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,100,D3D12_DESCRIPTOR_HEAP_FLAG_NONE ,0 }, dev),
            
            //�����������ַ���Cpu Descriptor���Ƶ������������У���Renderѭ����ֻ��Ҫͨ��SetDescriptorHeaps������������Gpu��
            //ע����Ϊcl->SetDescriptorHeaps���ͬʱ����һ��CSU��һ��SAMPLER Heap������������Heap��һ��ֻʹ�õ�һ�����ҵ�һ��Ҫ�㹻��
            csuHeap_Gpu("csuHeap_Gpu",1, { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,500,D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE ,0 }, dev),
            smpHeap_Gpu("smpHeap_Gpu",1, { D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,500,D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE ,0 }, dev)
        {}

        //��ÿ֡ĩβ���û��յ�������Դ
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