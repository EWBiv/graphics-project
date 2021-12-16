#include "ShaderIncludes.hlsli"

cbuffer ExternalData : register(b0)
{
	matrix view;
	matrix projection;
}

struct VertexToPixel_Skybox
{
	float4 position		: SV_POSITION;
	float3 sampleDir	: DIRECTION;
};

VertexToPixel_Skybox main(VertexShaderInput input)
{
	VertexToPixel_Skybox output;
	
	matrix vNoTranslation = view;
	vNoTranslation._14 = 0;
	vNoTranslation._24 = 0;
	vNoTranslation._34 = 0;
	
	//These two lines - I referenced the slides for a refresher on how exactly to apply everything correctly
	matrix viewProj = mul(projection, vNoTranslation);
	output.position = mul(viewProj, float4(input.localPosition, 1.0));

	output.position.z = output.position.w;
	output.sampleDir = input.localPosition;

	return output;
}

