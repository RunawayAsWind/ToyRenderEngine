#include "../../../Include/RTmpl/Container/BlockManager.h"
#include <cassert>
#include <iostream>
#include <algorithm>

namespace RTmpl
{
    BlockAllocation::~BlockAllocation()
    {

        if (resFather != nullptr)
        {
            //resFather->BlockWaitFree(range);
            //resFather->FreeBlock();
            resFather->BlockDirectFree(range);
        }

    }

    std::shared_ptr<BlockAllocation> BlockManager::Get(int num)
    {
        //��������num��������ѿ�������
        int bestIdx = -1;
        int minRest = INT_MAX;//��ѿ�������Ҫ��ȥnum��ʣ����С
        for (int i = 0; i < blockVec.size(); i++)
        {
            int rest = blockVec[i].second - blockVec[i].first + 1 - num;
            if (rest >= 0 && rest < minRest)
            {
                minRest = rest;
                bestIdx = i;
            }
        }

        if (bestIdx != -1)
        {
            int gloabalIdx = blockVec[bestIdx].first;              //��������������±� ��������Ϊ150~199, bFirst_wIdx=150

            //��������䱻�����ˣ��ڿ���������ɾ��������
            if (minRest == 0)
            {
                blockVec.erase(blockVec.begin() + bestIdx);
            }
            else//�������������ƣ���ʾǰ���һ���ֱ�ʹ��
            {
                blockVec[bestIdx].first += num;
            }

            std::pair<int, int> range = std::make_pair(gloabalIdx, gloabalIdx + num - 1);
            std::cout << name << " BlockManager::Get():" << range.first << " - " << range.second << std::endl;
            for (auto p : blockVec)
            {
                std::cout << "block: " << p.first << " - " << p.second << std::endl;
            }
            std::cout << std::endl;

            return std::make_shared<BlockAllocation>(this, range);
        }
        else
        {
            assert(num <= blockSize);
            Extend(1);
            return Get(num);
        }
    }

    void BlockManager::ShowInfo()
    {
        std::cout << name << " BlockManager::ShowInfo()" << std::endl;
        for (auto p : blockVec)
        {
            std::cout << "block: " << p.first << " - " << p.second << std::endl;
        }
        std::cout << std::endl;
    }

    //���DescritorHeap��Դ
    void BlockManager::Extend(int addBlockNum)
    {
        for (int i = 0; i < addBlockNum; i++)
        {
            int totalNum = blockNum * blockSize;
            AddBlock(std::make_pair(totalNum, totalNum + blockSize - 1));//��Ӷ�Ӧ��������
            blockNum++;
        }
    }

    void BlockManager::AddBlock(std::pair<int, int> p)
    {
        if (!MergeBlock(p))
        {
            blockVec.push_back(p);
            std::sort(blockVec.begin(), blockVec.end(), [](std::pair<int, int> a, std::pair<int, int> b) {return a.first < b.first; });
        }
    }

    //�ϲ����� ��p=11~39��blockVec������   0~10 40~99    100~199 �����øú����󣬱��    0~99      100~199   ͬһheap��������໥�ϲ�
    //            |Heap1|                |   Heap1   |  |  Heap2  |                   | Heap1 |  |  Heap2 |
    bool BlockManager::MergeBlock(std::pair<int, int> p)
    {
        bool success = false;
        int pHeapStartIdx = p.first / blockSize * blockSize;  //p����Heap��Heap�е�һ��desc����������world���𣩵��±�,����p = 11~39��pFirst_HeapFirstIdx =0; p = 150~189��pFirst_HeapFirstIdx =100;
        int pHeapEndIdx = pHeapStartIdx + blockSize - 1;
        int beforeIdx = -1;//��¼���Ժ�p�������ϲ�������     ��������0~10
        int afterIdx = -1;//��¼���Ժ�p�����Ҳ�ϲ�������      ��������40~99

        for (int i = 0; i < blockVec.size(); i++)
        {
            if (blockVec[i].first >= pHeapStartIdx && blockVec[i].second <= pHeapEndIdx)//����Ƚϵ�����Ҳ��ͬһHeap��
            {
                if (blockVec[i].second + 1 == p.first)
                {
                    beforeIdx = i;
                }
                if (p.second + 1 == blockVec[i].first)
                {
                    afterIdx = i;
                }
            }
        }
        if (beforeIdx != -1 && afterIdx != -1)
        {
            if (afterIdx < beforeIdx)
            {
                blockVec[afterIdx].first = blockVec[beforeIdx].first;
                blockVec.erase(blockVec.begin() + beforeIdx);
            }
            else
            {
                blockVec[beforeIdx].second = blockVec[afterIdx].second;
                blockVec.erase(blockVec.begin() + afterIdx);
            }
            success = true;
        }
        else if (beforeIdx != -1)
        {
            blockVec[beforeIdx].second = p.second;
            success = true;
        }
        else if (afterIdx != -1)
        {
            blockVec[afterIdx].first = p.first;
            success = true;
        }

        return success;
    }

    void BlockManager::BlockWaitFree(std::pair<int, int> p)
    {
        waitFree.push_back(p);
        std::cout << name << " BlockManager::BlockWaitFree(): " << p.first << " - " << p.second << std::endl;
        for (auto p : waitFree)
        {
            std::cout << "block: " << p.first << " - " << p.second << std::endl;
        }
        std::cout << std::endl;
    }

    void BlockManager::FreeBlock()
    {
        for (auto p : waitFree)
        {
            if (!MergeBlock(p))
            {
                blockVec.push_back(p);
            }
        }
        std::sort(blockVec.begin(), blockVec.end(), [](std::pair<int, int> a, std::pair<int, int> b) {return a.first < b.first; });
        waitFree.clear();
    }

    void BlockManager::BlockDirectFree(std::pair<int, int> p)
    {
        std::cout << name << " BlockManager::BlockDirectFree(): " << p.first << " - " << p.second << std::endl;
        if (!MergeBlock(p))
        {
            blockVec.push_back(p);
        }
    }
}