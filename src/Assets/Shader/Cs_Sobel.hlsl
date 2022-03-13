#ifndef __CS_SOBEL_HLSL__
#define __CS_SOBEL_HLSL__
#include "Design_Res.hlsl"
#include "Util_Math.hlsl"

// Approximates luminance ("brightness") from an RGB value.  These weights are derived from
// experiment based on eye sensitivity to different wavelengths of light.
float CalcLuminance(float3 color)
{
    return dot(color, float3(0.299f, 0.587f, 0.114f));
}

[numthreads(16, 16, 1)]
void CSMain(int3 dispatchThreadID : SV_DispatchThreadID)
{
    uint texW, texH, levels;
    gSobelInput.GetDimensions(0, texW, texH, levels);
    
    int sampleDis = 1;
    /**/
    // Sample the pixels in the neighborhood of this pixel.
    float4 c[3][3];
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int2 sampleIdx = dispatchThreadID.xy + int2(-1 + j, -1 + i) * sampleDis;
            sampleIdx = clamp(sampleIdx, 0, int2(texW - 1, texH - 1));

            float depthPercent = gSobelInput[sampleIdx].x;
            depthPercent = NdcDepthToViewDepth(depthPercent, gProj) / gFarZ;
            c[i][j] = float4(depthPercent, depthPercent, depthPercent, depthPercent);
        }
    }

	// For each color channel, estimate partial x derivative using Sobel scheme.
    float4 Gx = -1.0f * c[0][0] - 2.0f * c[1][0] - 1.0f * c[2][0] + 1.0f * c[0][2] + 2.0f * c[1][2] + 1.0f * c[2][2];

	// For each color channel, estimate partial y derivative using Sobel scheme.
    float4 Gy = -1.0f * c[2][0] - 2.0f * c[2][1] - 1.0f * c[2][1] + 1.0f * c[0][0] + 2.0f * c[0][1] + 1.0f * c[0][2];

	// Gradient is (Gx, Gy).  For each color channel, compute magnitude to get maximum rate of change.
    float4 mag = sqrt(Gx * Gx + Gy * Gy);

	// Make edges black, and nonedges white.
    mag = 1.0f - saturate(CalcLuminance(mag.rgb));

    gSobelOutput[dispatchThreadID.xy] = mag;
}

#endif