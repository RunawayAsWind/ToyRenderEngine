#pragma once
#include "FrBase.h"

namespace RDx12
{
    /*
    帧资源数组，vector储存单位帧资源
    */
    class FrVec
    {
    public:
        std::vector<FrBase> frResVec;

        void Create(ID3D12Device4* device, UINT frameNum)
        {
            for (int i = 0; i < frameNum; ++i)
            {
                frResVec.emplace_back(FrBase(device));
            }
            CbvSrvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }

        /*
        通过更新帧下标轮询访问vector中的单位帧资源
        */
        int UpdateFrResIdx()
        {
            nowFrResIndex = (nowFrResIndex + 1) % frResVec.size();
            return nowFrResIndex;
        }

        FrBase* GetNowFr()
        {
            return &(frResVec[nowFrResIndex]);
        }

        int GetNowFrIdx()
        {
            return nowFrResIndex;
        }

		int GetFrNum()
		{
			return frResVec.size();
		}

        void ShowFrFeNumInfo()
        {
            for (int i = 0; i < frResVec.size(); i++)
            {
                WIN32DEBUG(L"fr%d ", i);
            }
            WIN32DEBUG(L"\n");
            for (int i = 0; i < frResVec.size(); i++)
            {
                WIN32DEBUG(L" %d  ", frResVec[i].GetFenceNum());
            }
            WIN32DEBUG(L"\n");
        }

    private:
        int nowFrResIndex = 0;
        UINT CbvSrvDescriptorSize = 0;

    };
}