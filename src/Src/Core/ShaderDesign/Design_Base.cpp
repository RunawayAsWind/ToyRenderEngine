#include "../../../Include/Core/ShaderDesign/Design_Base.h"

PassCb GetPassCb(
    DirectX::XMFLOAT3 gEyePosW,
    DirectX::XMMATRIX gView,
    DirectX::XMMATRIX gProj,
    DirectX::XMFLOAT2 gRenderTargetSize,
    DirectX::XMFLOAT2 gNearSize,
    float gNearZ,
    float gFarZ,
    float gTotalTime,
    float gDeltaTime)
{
    DirectX::XMMATRIX viewProj = DirectX::XMMatrixMultiply(gView, gProj);
    DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(&XMMatrixDeterminant(gView), gView);
    DirectX::XMMATRIX invProj = DirectX::XMMatrixInverse(&XMMatrixDeterminant(gProj), gProj);
    DirectX::XMMATRIX invViewProj = DirectX::XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

    PassCb passCb;
    DirectX::XMStoreFloat4x4(&passCb.gView, DirectX::XMMatrixTranspose(gView));
    DirectX::XMStoreFloat4x4(&passCb.gInvView, DirectX::XMMatrixTranspose(invView));
    DirectX::XMStoreFloat4x4(&passCb.gProj, DirectX::XMMatrixTranspose(gProj));
    DirectX::XMStoreFloat4x4(&passCb.gInvProj, DirectX::XMMatrixTranspose(invProj));
    DirectX::XMStoreFloat4x4(&passCb.gViewProj, DirectX::XMMatrixTranspose(viewProj));
    DirectX::XMStoreFloat4x4(&passCb.gInvViewProj, DirectX::XMMatrixTranspose(invViewProj));

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	DirectX::XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	DirectX::XMStoreFloat4x4(&passCb.gProjTex, DirectX::XMMatrixTranspose(gProj * T));

    passCb.gEyePosW = gEyePosW;
    passCb.gRenderTargetSize = gRenderTargetSize;
    passCb.gInvRenderTargetSize = { 1.0f / passCb.gRenderTargetSize.x,1.0f / passCb.gRenderTargetSize.y };
    passCb.gNearSize = gNearSize;
    passCb.gFarSize = { gNearSize.x * gFarZ / gNearZ,gNearSize.y * gFarZ / gNearZ };
    passCb.gNearZ = gNearZ;
    passCb.gFarZ = gFarZ;
    passCb.gTotalTime = gTotalTime;
    passCb.gDeltaTime = gDeltaTime;

    return passCb;
}


ObjCb GetObjCb(
    DirectX::XMMATRIX gWorld,
    DirectX::XMMATRIX gTexTransform,
	int gRenderIdx,
	int gObjTexCbIdx,
	int gIsSelected,
	int gPadding2)
{
    ObjCb objCb;
    DirectX::XMStoreFloat4x4(&objCb.gWorld, DirectX::XMMatrixTranspose(gWorld));
    DirectX::XMStoreFloat4x4(&objCb.gNormalWorld, DirectX::XMMatrixTranspose(RDx12::MathHelper::InverTMat(gWorld)));
    DirectX::XMStoreFloat4x4(&objCb.gTexTransform, DirectX::XMMatrixTranspose(gTexTransform));
	objCb.gRenderIdx = gRenderIdx;
	objCb.gObjTexCbIdx = gObjTexCbIdx;
	objCb.gIsSelected = gIsSelected;
	objCb.gPadding2 = gPadding2;
    return objCb;
};

LightCb GetLightCb(
    std::vector<Light>& directLightVec,
    std::vector<Light>& pointLightVec,
    std::vector<Light>& spotLightVec)
{
    LightCb lightCb;
    for (auto& light : directLightVec)
    {
        if (lightCb.gDirLightCount < 4)
            lightCb.gDirLight[lightCb.gDirLightCount++] = light;
        else
            break;
    }
    for (auto& light : pointLightVec)
    {
        if (lightCb.gPointLightCount < 16)
            lightCb.gPointLight[lightCb.gPointLightCount++] = light;
        else
            break;
    }
    for (auto& light : spotLightVec)
    {
        if (lightCb.gSpotLightCount < 16)
            lightCb.gSpotLight[lightCb.gSpotLightCount++] = light;
        else
            break;
    }
    return lightCb;
}