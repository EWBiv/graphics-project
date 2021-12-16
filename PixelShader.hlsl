#include "ShaderIncludes.hlsli"

Texture2D Albedo		: register(t0);	//'t' -> textures
Texture2D NormalMap		: register(t1);
Texture2D RoughnessMap	: register(t2);
Texture2D MetalnessMap	: register(t3);

SamplerState BasicSampler : register(s0);	//'s' -> samplers

cbuffer ExternalData : register(b0)
{
	Light lights[6];
	float3 ambient;
	float4 colorTint;
	float3 cameraPosition;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//	Lighting helper functions - could be moved to ShaderIncludes at some point certainly	/////
/////////////////////////////////////////////////////////////////////////////////////////////////

//float3 Diffuse(float3 dirToLight, float3 normal, Light light)
//{
//	float3 diffuse = saturate(dot(normal, dirToLight))  * light.Color * light.Intensity;
//	return diffuse;
//}

																	//Isn't always attached to light (point lights), so passed in as well
//float Specular(float specularExp, float3 normal, float3 V, Light light, float3 lightDir)
//{
//	float specular = 0.0f;
//
//	//Uses R, which needs to be recalculated per light
//	if (specularExp > 0.0f)
//	{
//		float3 R = reflect(normalize(lightDir), normal);
//		specular = pow(saturate(dot(R, V)), specularExp) * light.Color * light.Intensity;
//	}
//
//	return specular;
//}

float Attenuate(Light light, float3 worldPos)
{
	float dist = distance(light.Position, worldPos);
	float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
	return att * att;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	
	//Rudimentary blend
	//float3 blendColor = float3(0, 0, 0);
	//blendColor = BlendedTexture.Sample(BasicSampler, input.uv).rgb / 8;

	//Get the matrix of vectors ready
	float3 N = normalize(input.normal);		//Renormalizing each vector passed - interpolation (of pixels) can lead to them being un-normalized by a bit
	float3 T = normalize(input.tangent);
	T = normalize(T - N * dot(T, N));		//Gram-Schmidt orthonormalize -> assumes T&N are normalized!
	float3 B = cross(T, N);					//Bi-tangent
	float3x3 TBN = float3x3(T, B, N);

	//Normal from texture
	float3 unpackedNormal = NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1;
	input.normal = mul(unpackedNormal, TBN);
	
																	    
	float3 surfaceColor = pow(Albedo.Sample(BasicSampler, input.uv).rgb, 2.2f) * colorTint; //Getting texture color

	float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r; //Greyscale, so getting any color channel works here
	float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r; // ''


	float3 ambientAmount = float3(0,0,0);		//Ambient color - ambient color * surface color
	//float specularExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
	float3 viewVector = normalize((cameraPosition - input.worldPosition));
	

	float3 finalPixelTint = float3(0,0,0);

	//Assume albedo texture is holding specular color where metalness == 1
	//
	//Metal normally 0 or 1, but might be in between so a linear interpolation is in line
	float3 specColor = lerp(F0_NON_METAL.rrr, surfaceColor.rgb, metalness);

	//Things that need to be repeated per light
	for (int i = 0; i < 6; i++)
	{			
		//Setting up values to be modifying depending on the light
		float3 dirToLight = float3(0, 0, 0);
		float3 lightsDirection = float3(0, 0, 0);
		float attenuation = 1;				//Has no affect on end result as it multiplys by 1, unless it is a point light

		//Need to get the direction + factor in attenuation
		if (lights[i].Type == LIGHT_TYPE_POINT)
		{
			lightsDirection = normalize((input.worldPosition - lights[i].Position));
			dirToLight = -lightsDirection;

			attenuation = Attenuate(lights[i], input.worldPosition);
		}
		else
		{
			lightsDirection = lights[i].Direction;

			dirToLight = -lightsDirection;
			dirToLight = normalize(dirToLight);
		}

		//Spec + diffuse - 'light amounts'
		float3 diffuse = DiffusePBR(input.normal,dirToLight);
		float3 spec = MicrofacetBRDF(input.normal, dirToLight, viewVector, roughness, specColor);
		

		//Diffuse taking in account energy conservation (reflected light not diffused)
		float3 balancedDiff = DiffuseEnergyConserve(diffuse, spec, metalness);

		//Combine the final diffuse and specular values for this light
		//spec *= any(diffuse);
		float3 totalLightOutput = (surfaceColor * balancedDiff + spec) * lights[i].Color * lights[i].Intensity;


		finalPixelTint += (totalLightOutput) * attenuation;
	}

	//Add ambient at end - after all other lights have been calculated (order not really important, just adding the ambient light to the 'final' tint)
	finalPixelTint += ambientAmount;

	return float4(pow(finalPixelTint, 1.0f / 2.2f),1);
}