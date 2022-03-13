#ifndef __VS_GBUFFER_HLSL__
#define __VS_GBUFFER_HLSL__
#include "Design_Res.hlsl"

VSOut_Gbuffer VSMain(VSIn_Default vin)
{
    VSOut_Gbuffer vout;
#ifdef SKINNED
    //该顶点至少需要有一个绑定骨骼才进行蒙皮计算
    if(vin.BoneIndices[0]!=-1)
    {  
        float3 posL, normalL, tangentL;
        float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        weights[0] = vin.BoneWeights.x;
        weights[1] = vin.BoneWeights.y;
        weights[2] = vin.BoneWeights.z;
        weights[3] = 1 - weights[0] - weights[1] - weights[2];
        //一个顶点最多由4个骨骼控制
        for (int i = 0; i < 4; ++i)
        {
            //到-1表示后面没有骨骼了，提前结束循环
            if(vin.BoneIndices[i]==-1) {break;}
            posL += weights[i] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices[i]]).xyz;
            normalL += weights[i] * mul(vin.NormalL, (float3x3) gBoneTransforms[vin.BoneIndices[i]]);
            tangentL += weights[i] * mul(vin.TangentL, (float3x3) gBoneTransforms[vin.BoneIndices[i]]);
        }
        vout.PosH = mul(float4(posL, 1.0f),gViewProj);
        vout.NormalW = normalL;
        vout.TangentW = tangentL;
    }
    else
    {
        vout.PosH = mul(mul(float4(vin.PosL, 1.0f), gWorld), gViewProj);
        vout.NormalW = mul(vin.NormalL, (float3x3) gNormalWorld);
        vout.TangentW = mul(vin.TangentL, (float3x3) gWorld);
    }
#else
    vout.PosH = mul(mul(float4(vin.PosL, 1.0f), gWorld), gViewProj);
    vout.NormalW = mul(vin.NormalL, (float3x3) gNormalWorld);
    vout.TangentW = mul(vin.TangentL, (float3x3) gWorld);
#endif  
    vout.TexC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform).xy;
    return vout;
}

#endif