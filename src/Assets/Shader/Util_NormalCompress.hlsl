#ifndef __UTIL_NORMALCOMPRESS_HLSL__
#define __UTIL_NORMALCOMPRESS_HLSL__
//Octahedral Normal Vectors
float2 NormalCompress_UnitVectorToOctahedron(float3 N)
{
    N.xy /= dot(1, abs(N)); //八面体公式是|x|+|y|+|z|=1 ,dot(1, abs(N))=|x|+|y|+|z|,保证了绝对值和=1
    if (N.z <= 0)
    {
        N.xy = (1 - abs(N.yx)) * (N.xy >= 0 ? float2(1, 1) : float2(-1, -1));
    }
    return N.xy;
    
}

float3 NormalCompress_OctahedronToUnitVector(float2 cN)
{
    float3 N = float3(cN, 1 - dot(1, abs(cN)));
    if (N.z < 0)
    {
        N.xy = (1 - abs(N.yx)) * (N.xy >= 0 ? float2(1, 1) : float2(-1, -1));
    }
    return normalize(N);
}

//Spherical Coordinates
static const float kPI = 3.14159265359f;
float2 NormalCompress_UnitVectorToSpCoord(float3 N)
{
    return float2(atan2(N.y, N.x) / kPI, N.z);
    
}

float3 NormalCompress_SpCoordToUnitVector(float2 cN)
{
    float2 scth;
    sincos(cN.x * kPI, scth.x, scth.y);
    float2 scphi = float2(sqrt(1.0 - cN.y * cN.y), cN.y);
    return float3(scth.y * scphi.x, scth.x * scphi.x, scphi.y);
}

//Stereographic Projection
//https://zhuanlan.zhihu.com/p/126345392
float2 NormalCompress_UnitVectorToStProj(float3 N)
{
    return float2(N.x / (1 - N.z), N.y / (1 - N.z));
    
}

float3 NormalCompress_StProjToUnitVector(float2 cN)
{
    return float3(2 * cN.x / (1 + cN.x * cN.x + cN.y * cN.y), 2 * cN.y / (1 + cN.x * cN.x + cN.y * cN.y), (-1 +cN.x * cN.x + cN.y * cN.y) / (1 + cN.x * cN.x + cN.y * cN.y));
}

//Spheremap Transform
float2 NormalCompress_UnitVectorToSpTrans(float3 N)
{
    return float2(sqrt(2 / (1 - N.z))* N.x, sqrt(2 / (1 - N.z))* N.y);
    
}

float3 NormalCompress_SpTransToUnitVector(float2 cN)
{
    return float3(sqrt(1 - (cN.x * cN.x + cN.y * cN.y) / 4) * cN.x, sqrt(1 - (cN.x * cN.x + cN.y * cN.y) / 4) * cN.y, -1 + (cN.x * cN.x + cN.y * cN.y) / 2);
}

#endif