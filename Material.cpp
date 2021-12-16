#include "Material.h"
																								//, float _roughness
Material::Material(SimplePixelShader* pShader, SimpleVertexShader* vShader, DirectX::XMFLOAT4 tint) :
	pixelShader(pShader), vertexShader(vShader), colorTint(tint)
{

}

Material::~Material()
{
}

SimplePixelShader* Material::GetPixelShader()
{
	return pixelShader;
}

SimpleVertexShader* Material::GetVertexShader()
{
	return vertexShader;
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
	return colorTint;
}

//float Material::GetRoughness()
//{
//	return roughness;
//}

void Material::SetPixelShader(SimplePixelShader* pShader)
{
	pixelShader = pShader;
}

void Material::SetVertexShader(SimpleVertexShader* vShader)
{
	vertexShader = vShader;
}

void Material::SetColorTint(DirectX::XMFLOAT4 tint)
{
	colorTint = tint;
}

//void Material::SetRoughness(float _roughness)
//{
//	//Clamping roughness to between 0 and 1
//	if (_roughness > 1.0f) _roughness = 1.0f;
//	if (_roughness < 0.0f) _roughness = 0.0f;
//
//	roughness = _roughness;
//}

void Material::AddTextureSRV(std::string textureName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV)
{
	textureSRVs.insert({ textureName,textureSRV });
}

void Material::AddSampler(std::string samplerName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
	samplers.insert({ samplerName,sampler });
}

void Material::ReadyTexture()
{
	for (auto& t : textureSRVs)
		pixelShader->SetShaderResourceView(t.first.c_str(), t.second);

	for (auto& s : samplers)
		pixelShader->SetSamplerState(s.first.c_str(), s.second);
}

