#include "../Include/RDx12/ResCreate/DescHeap/DescHeapPool.h"

namespace RDx12
{
    std::shared_ptr<HeapAllocation> DescHeapPool::Get(int num)
    {
        auto blockAlloction = blockManager.Get(num);
        auto range = blockAlloction->range;

        //����������ڵڼ���heap ��ÿ��heap��100��descriptor�� ����150~199����150/100=1 ������ heapVec[1]��descheap
        int hpIdx = range.first / desc.NumDescriptors;

        while (hpIdx >= heapVec.size())
        {
            Extend(1);
        }

        //��ø�desc��heap�ڲ����±꣬ ��wIdx=150 ÿ��heap��100��desc����belongHeapIdx=1��hpIdx=50    ��wIdx=150  ---> heapVec[1]->hCpu(50)
        int localHpDesStart = range.first - range.first / desc.NumDescriptors * desc.NumDescriptors;
        std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> hCpu;
        std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> hGpu;

        //����num����heapIdx������仯��ÿ�����䶼��Ȼ����ͬ��heap(��������ϲ���ʱ��ֻ�ϲ�ͬheap�е�����)
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

    //���DescritorHeap��Դ
    void DescHeapPool::Extend(UINT addHeapNum)
    {
        for (UINT i = 0; i < addHeapNum; i++)
        {
            UINT totalNum = heapVec.size() * desc.NumDescriptors;
            DescHeapCl newDescHeap;
            newDescHeap.Create(desc, dev.Get());
            heapVec.emplace_back(std::move(newDescHeap));//���descheap
            blockManager.Extend(1);//��Ӷ�Ӧ��������
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