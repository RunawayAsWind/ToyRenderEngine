#pragma once
#include <RTmpl/Container/BlockManager.h>
#include "DescHeapCl.h"


namespace RDx12 {

    class HeapAllocation;

    class DescHeapPool
    {
    public:
        DescHeapPool(std::string name,UINT initHeapNum, D3D12_DESCRIPTOR_HEAP_DESC desc, Microsoft::WRL::ComPtr<ID3D12Device4> dev) :blockManager(name,desc.NumDescriptors),desc(desc), dev(dev) { Extend(initHeapNum); }
        std::shared_ptr<HeapAllocation> Get(int num);//从池中获得堆
        void SetByCmdList(UINT idx,ID3D12GraphicsCommandList* cl);//内部调用cl->SetDescriptorHeaps，仅用于Usc Gpu 和 Sample Gpu
        void ShowInfo();

    private:
        RTmpl::BlockManager blockManager;
        D3D12_DESCRIPTOR_HEAP_DESC desc;
        Microsoft::WRL::ComPtr<ID3D12Device4> dev;
        std::vector<DescHeapCl> heapVec;                //负责提供Descriptor的堆
        void Extend(UINT addHeapNum);                   //增加堆
        void AddBlock(std::pair<UINT, UINT> p);         //增加可用区块（回收用）
        void BlockWaitFree(std::pair<UINT, UINT> p);    //回收资源的时候先将回收区间push入待回收队列waitFree
        void FreeBlock();                               //在每帧末尾调用回收掉所有资源，会将waitFree中所有区块推入blockVec
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