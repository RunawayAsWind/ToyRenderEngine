#pragma once
#include "../../Core/ShaderDesign/ShaderManager.h"
#include "Cmpt_Trans.h"
#include "Cmpt_Camera.h"

struct Cmpt_PassCb
{   
    RDx12::FrameResAlloc_RootCb<PassCb> mPassCb;
    
    static std::function<void(Cmpt_PassCb*, Cmpt_Camera*, Cmpt_Trans*)> Update(RDx12::FrVec& fv);
   
    void AllocRes(RDx12::Sign_RootCb<PassCb>& sd_PassCb);
    void PreUpdate(DirectX::XMFLOAT2 RenderTargetSize, DirectX::XMFLOAT2 InvRenderTargetSize,float totalTime,float deltaTime);
    void SetGraphicsRootConstantBufferView(ID3D12GraphicsCommandList* cl, RDx12::FrVec& fv);
	void SetComputeRootConstantBufferView(ID3D12GraphicsCommandList* cl, RDx12::FrVec& fv);

    void RSerialize_Save(std::ofstream& outFile) {};
    void RSerialize_Load(std::ifstream& inFile) {};
};