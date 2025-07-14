#include "Layouts.hlsli"

float4 main(VSOutput_Offset input) : SV_TARGET
{
    float dst = sqrt(dot(input.Offset, input.Offset));
    
    if (dst >= 1.0)
    {
        discard;
    }
    
    return float4(1.0, 1.0, 1.0, 1.0);
}