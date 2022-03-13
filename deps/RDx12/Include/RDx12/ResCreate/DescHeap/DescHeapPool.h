#pragma once
#include <RTmpl/Container/BlockManager.h>
#include "DescHeapCl.h"


namespace RDx12 {

    class HeapAllocation;

    class DescHeapPool
    {
    public:
        DescHeapPool(std::string name,UINT initHeapNum, D3D12_DESCRIPTOR_HEAP_DESC desc, Microsoft::WRL::ComPtr<ID3D12Device4> dev) :blockManager(name,desc.NumDescriptors),desc(desc), dev(dev) { Extend(initHeapNum); }
        std::shared_ptr<HeapAllocation> Get(int num);//�ӳ��л�ö�
        void SetByCmdList(UINT idx,ID3D12GraphicsCommandList* cl);//�ڲ�����cl->SetDescriptorHeaps��������Usc Gpu �� Sample Gpu
        void ShowInfo();

    private:
        RTmpl::BlockManager blockManager;
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        Microsoft::WRL::ComPtr<ID3D12Device4> dev;
        std::vector<DescHeapCl> heapVec;                //�����ṩDescriptor�Ķ�
        void Extend(UINT addHeapNum);                   //���Ӷ�
        void AddBlock(std::pair<UINT, UINT> p);         //���ӿ������飨�����ã�
        void BlockWaitFree(std::pair<UINT, UINT> p);    //������Դ��ʱ���Ƚ���������push������ն���waitFree
        void FreeBlock();                               //��ÿ֡ĩβ���û��յ�������Դ���ὫwaitFree��������������blockVec
        friend class HeapAllocation;
        friend class HeapManager;
    };

    class HeapAllocation
    {
    private:
        std::shared_ptr<RTmpl::BlockAllocation> blockAlloc;
    public:
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> hCpu;
        std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> hGpu;
        HeapAllocation(std::shared_ptr<RTmpl::BlockAllocation> blockAlloc, std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>&& hCpu, std::vector<D3D12_GPU_DESCRIPTOR_HANDLE>&& hGpu) :blockAlloc(blockAlloc), hCpu(std::move(hCpu)), hGpu(std::move(hGpu)) {}
    };

   
}