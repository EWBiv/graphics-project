#include "ShaderIncludes.hlsli"

TextureCube SkyTexture : register(t0); //'t' -> textures

SamplerState SkySampler : register(s0);	//'s' -> samplers

struct VertexToPixel_Skybox
{
	float4 position		: SV_POSITION;
	float3 sampleDir	: DIRECTION;
};

float4 main(VertexToPixel_Skybox input) : SV_TARGET
{
	float3 skyPixel = SkyTexture.Sample(SkySampler, input.sampleDir);
	//return float4(skyPixel, 1);
	return float4(skyPixel, 1);
}

