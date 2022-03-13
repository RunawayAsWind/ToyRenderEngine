#pragma once
#include "HeapManager.h"
#include "../ShaderSign/SdSign_Cbv.h"
#include "../ShaderSign/SdSign_Srv.h"

namespace RDx12 {

    class GpuHandler
    {
    public:
        GpuHandler() {}

        void Init(
            std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> hcpuVec,
            UINT rpIdx,
            RDx12::HeapManager& hpManager)
        {
            assert(hcpuVec.size() > 0);
			mRpIdx=rpIdx;
            if (mHpAlloc_CsuGpu == nullptr || mHpAlloc_CsuGpu->hCpu.size() < hcpuVec.size()) {
                mHpAlloc_CsuGpu = hpManager.csuHeap_Gpu.Get(hcpuVec.size());
            }
			assert(mHpAlloc_CsuGpu != nullptr);

            for (int i = 0; i < hcpuVec.size(); i++)
            {
                if (hcpuVec[i].ptr != 0)
                {
                    hpManager.GetDev()->CopyDescriptorsSimple(1, mHpAlloc_CsuGpu->hCpu[i], hcpuVec[i], D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                }
            }
        }

        void Change(UINT idx, D3D12_CPU_DESCRIPTOR_HANDLE hCpu,ID3D12Device* dev)
        {
            dev->CopyDescriptorsSimple(1, mHpAlloc_CsuGpu->hCpu[idx], hCpu, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }

        void SetGpuData(int idx,ID3D12GraphicsCommandList* cl)
        {
            cl->SetGraphicsRootDescriptorTable(mRpIdx, mHpAlloc_CsuGpu->hGpu[idx]);
        }

    private:
        std::shared_ptr<RDx12::HeapAllocation> mHpAlloc_CsuGpu;
        UINT mRpIdx;
    };
}