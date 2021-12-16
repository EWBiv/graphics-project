#pragma once

#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <DirectXMath.h>
#include <d3d11.h>
#include "SimpleShader.h"
#include <unordered_map>


class Material
{
public:																					//, float _roughness
	Material(SimplePixelShader* pShader, SimpleVertexShader* vShader, DirectX::XMFLOAT4 tint);
	~Material();

	//Getters
	SimplePixelShader* GetPixelShader();
	SimpleVertexShader* GetVertexShader();
	DirectX::XMFLOAT4 GetColorTint();
	//float GetRoughness();

	//Setters - not sure each is absolutely necessary right now
	void SetPixelShader(SimplePixelShader* pShader);
	void SetVertexShader(SimpleVertexShader* vShader);
	void SetColorTint(DirectX::XMFLOAT4 tint);
	//void SetRoughness(float _roughness);

	void AddTextureSRV(std::string textureName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV);
	void AddSampler(std::string samplerName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);
	void ReadyTexture();

private:
	

	//float roughness;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShader;
	DirectX::XMFLOAT4 colorTint;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureSRVs;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;
};

