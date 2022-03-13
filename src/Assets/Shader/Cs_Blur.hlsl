#ifndef __CS_HORZBLUR_HLSL__
#define __CS_HORZBLUR_HLSL__
#include "Design_Res.hlsl"

static const int gMaxBlurRadius = 5;

#define N 256
#define CacheSize (N + 2*gMaxBlurRadius)
groupshared float4 gCache[CacheSize];

[numthreads(N, 1, 1)]
void CSMain_HorzBlur(int3 groupThreadID : SV_GroupThreadID,
				int3 dispatchThreadID : SV_DispatchThreadID)
{
    // Put in an array for each indexing.
    float weights[11] = { gBlurWeight0, gBlurWeight1, gBlurWeight2, gBlurWeight3, gBlurWeight4, gBlurWeight5, gBlurWeight6, gBlurWeight7, gBlurWeight8, gBlurWeight9, gBlurWeight10 };
    
    uint texW, texH,levels;
    gBlurInput.GetDimensions(0, texW, texH, levels);
    //
    // Fill local thread storage to reduce bandwidth.  To blur 
    // N pixels, we will need to load N + 2*BlurRadius pixels
    // due to the blur radius.
    //
    
    /*
    
    | gMaxBlurRadius |                                    N                                    | gMaxBlurRadius |
                     | gMaxBlurRadius |                                       | gMaxBlurRadius |
                     |    采样2次     |               采样一次                |    采样两次    |
    */
    // This thread group runs N threads.  To get the extra 2*BlurRadius pixels, 
    // have 2*BlurRadius threads sample an extra pixel.
    if (groupThreadID.x < gBlurRadius)
    {
    	// Clamp out of bound samples that occur at image borders.
        int x = max(dispatchThreadID.x - gBlurRadius, 0);
        gCache[groupThreadID.x] = gBlurInput[int2(x, dispatchThreadID.y)];
    }
    if (groupThreadID.x >= N - gBlurRadius)
    {
    	// Clamp out of bound samples that occur at image borders.
        int x = min(dispatchThreadID.x + gBlurRadius, texW - 1);
        gCache[groupThreadID.x + 2 * gBlurRadius] = gBlurInput[int2(x, dispatchThreadID.y)];
    }
    
    // Clamp out of bound samples that occur at image borders.
    gCache[groupThreadID.x + gBlurRadius] = gBlurInput[min(dispatchThreadID.xy, int2(texW,texH) - 1)];//这里-1时因为索引是从0开始，例如texW为100，最后一个索引为99
    
    // Wait for all threads to finish.
    GroupMemoryBarrierWithGroupSync();
    
    //
    // Now blur each pixel.
    //
    
    float4 blurColor = float4(0, 0, 0, 0);
    
    for (int i = -gBlurRadius; i <= gBlurRadius; ++i)
    {
        int k = groupThreadID.x + gBlurRadius + i;
    
        blurColor += weights[i + gBlurRadius] * gCache[k];
    }
    
    gBlurOutput[dispatchThreadID.xy] = blurColor;
}

[numthreads(1, N, 1)]
void CSMain_VertBlur(int3 groupThreadID : SV_GroupThreadID,
				int3 dispatchThreadID : SV_DispatchThreadID)
{
     // Put in an array for each indexing.
    float weights[11] = { gBlurWeight0, gBlurWeight1, gBlurWeight2, gBlurWeight3, gBlurWeight4, gBlurWeight5, gBlurWeight6, gBlurWeight7, gBlurWeight8, gBlurWeight9, gBlurWeight10 };
    
    uint texW, texH, levels;
    gBlurInput.GetDimensions(0, texW, texH, levels);
    //
    // Fill local thread storage to reduce bandwidth.  To blur 
    // N pixels, we will need to load N + 2*BlurRadius pixels
    // due to the blur radius.
    //
    
    // This thread group runs N threads.  To get the extra 2*BlurRadius pixels, 
    // have 2*BlurRadius threads sample an extra pixel.
    if (groupThreadID.y < gBlurRadius)
    {
    	// Clamp out of bound samples that occur at image borders.
        int y = max(dispatchThreadID.y - gBlurRadius, 0);
        gCache[groupThreadID.y] = gBlurInput[int2(dispatchThreadID.x, y)];
    }
    if (groupThreadID.y >= N - gBlurRadius)
    {
    	// Clamp out of bound samples that occur at image borders.
        int y = min(dispatchThreadID.y + gBlurRadius, texH - 1);
        gCache[groupThreadID.y + 2 * gBlurRadius] = gBlurInput[int2(dispatchThreadID.x, y)];
    }
    
    // Clamp out of bound samples that occur at image borders.
    gCache[groupThreadID.y + gBlurRadius] = gBlurInput[min(dispatchThreadID.xy, int2(texW, texH) - 1)];
    
    
    // Wait for all threads to finish.
    GroupMemoryBarrierWithGroupSync();
    
    //
    // Now blur each pixel.
    //
    
    float4 blurColor = float4(0, 0, 0, 0);
    
    for (int i = -gBlurRadius; i <= gBlurRadius; ++i)
    {
        int k = groupThreadID.y + gBlurRadius + i;
    
        blurColor += weights[i + gBlurRadius] * gCache[k];
    }
    
    gBlurOutput[dispatchThreadID.xy] = blurColor;
}

#endif