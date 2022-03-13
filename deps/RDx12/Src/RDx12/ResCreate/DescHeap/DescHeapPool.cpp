#include "../Include/RDx12/ResCreate/DescHeap/DescHeapPool.h"

namespace RDx12
{
    std::shared_ptr<HeapAllocation> DescHeapPool::Get(int num)
    {
        auto blockAlloction = blockManager.Get(num);
        auto range = blockAlloction->range;

        //最佳区间属于第几个heap 如每个heap有100个descriptor， 区块150~199属于150/100=1 即属于 heapVec[1]的descheap
        int hpIdx = range.first / desc.NumDescriptors;

        while (hpIdx >= heapVec.size())
        {
            Extend(1);
        }

        //获得该desc在heap内部的下标， 如wIdx=150 每个heap有100个desc，则belongHeapIdx=1，hpIdx=50    即wIdx=150  ---> heapVec[1]->hCpu(50)
        int localHpDesStart = range.first - range.first / desc.NumDescriptors * desc.NumDescriptors;
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> hCpu;
        std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> hGpu;

        //随着num增加heapIdx并不会变化，每个区间都必然属于同个heap(后面区间合并的时候只合并同heap中的区块)
        for (int i = 0; i < num; i++)
        {
            hCpu.push_back(heapVec[hpIdx].hCPU(localHpDesStart + i));
            if (desc.Flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
            {
                hGpu.push_back(heapVec[hpIdx].hGPU(localHpDesStart + i));
            }
        }

        return std::make_shared<HeapAllocation>(blockAlloction, std::move(hCpu), std::move(hGpu));
    }

    void DescHeapPool::SetByCmdList(UINT idx, ID3D12GraphicsCommandList* cl)
    {
        heapVec[idx].SetByCmdList(cl);
    }

    void DescHeapPool::ShowInfo()
    {
        blockManager.ShowInfo();
    }

    //添加DescritorHeap资源
    void DescHeapPool::Extend(UINT addHeapNum)
    {
        for (UINT i = 0; i < addHeapNum; i++)
        {
            UINT totalNum = heapVec.size() * desc.NumDescriptors;
            DescHeapCl newDescHeap;
            newDescHeap.Create(desc, dev.Get());
            heapVec.emplace_back(std::move(newDescHeap));//添加descheap
            blockManager.Extend(1);//添加对应空闲区块
        }
    }

    void DescHeapPool::AddBlock(std::pair<UINT, UINT> p)
    {
        blockManager.AddBlock(p);
    }
    
    
    void DescHeapPool::BlockWaitFree(std::pair<UINT, UINT> p)
    {
        blockManager.BlockWaitFree(p);
    }

    void DescHeapPool::FreeBlock()
    {
        blockManager.FreeBlock();
    }
}