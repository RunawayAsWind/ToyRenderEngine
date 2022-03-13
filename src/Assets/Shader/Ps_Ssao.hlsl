#ifndef __PS_SSAO_HLSL__
#define __PS_SSAO_HLSL__
#include "Design_Res.hlsl"
#include "Util_Sample.hlsl"
#include "Util_NormalCompress.hlsl"

static const float gOcclusionRadius = 0.3f;
static const float gOcclusionFadeStart = 0.2f;
static const float gOcclusionFadeEnd = 1.0f;
static const float gSurfaceEpsilon = 0.05f;

float PSMain(VSOut_ScreenQuat pin) : SV_TARGET
{
    /*
    float2 encodeN = gNormalBuffer.Sample(gsamAnisotropicWrap, pin.TexC).xy;
    if(encodeN.x==0 && encodeN.y==0)
    {
        return 0;
    }
    
    float3 viewN = mul(NormalCompress_SpTransToUnitVector(encodeN), (float3x3) gView);
    */
    
    //观察空间中的法线
    float3 N = gNormalBuffer.Sample(gsamAnisotropicWrap, pin.TexC).xyz;
    if(N.x==0 && N.y==0 && N.z==0)
    {
        return 0;
    }
    float3 viewN = mul(N, (float3x3) gView);
    
    //像素点在观察空间中的位置
    float depthPercent = NdcDepthToViewDepth(gDepthMap.Sample(gsamDepthMapSam, pin.TexC).r, gProj) / gFarZ;
    float3 P_viewPos = pin.PosV * depthPercent;

    float occlusionSum = 0.0f;

    float3 up = float3(0.0, 1.0, 0.0);
    float3 right = cross(up, viewN);
    up = cross(viewN, right);
    
    uint sampleNum = 8u;
    for (uint i = 0u; i < sampleNum; i++)
    {
      float2 Xi = Random_Hammersley2d(i, sampleNum);
      float3 hemiSampleL_Tan = Sample_HemiSphere_Unifom(Xi.x, Xi.y);
     
      float3 sampleVec = hemiSampleL_Tan.x * right + hemiSampleL_Tan.y * up + hemiSampleL_Tan.z * viewN;
      float3 samplePos = P_viewPos + sampleVec * gOcclusionRadius * Xi.y;
      
      float4 projQ = mul(float4(samplePos, 1.0f), gProjTex);
      projQ /= projQ.w;
    
      
      float3 R_viewPos = samplePos * NdcDepthToViewDepth(gDepthMap.Sample(gsamDepthMapSam, projQ.xy).r, gProj) / samplePos.z;
      
      //测试点R是否遮挡点P
      float distZ = P_viewPos.z - R_viewPos.z;
    
      float dp = max(dot(viewN, normalize(R_viewPos - P_viewPos)), 0.0f);
      float occlusion = dp * OcclusionFunction(gSurfaceEpsilon, distZ, gOcclusionFadeStart, gOcclusionFadeEnd);
      occlusionSum += occlusion; 
    }
    occlusionSum /= sampleNum;
    float access = 1.0 - occlusionSum;

    return saturate(pow(access, 2.0f));
}

#endif