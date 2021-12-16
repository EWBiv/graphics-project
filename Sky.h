#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects

#include "SimpleShader.h"
#include "Mesh.h"
#include "Camera.h"

#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"



class Sky
{
public:
	Sky(Mesh* mesh, SimpleVertexShader* vShader, SimplePixelShader* pShader, Microsoft::WRL::ComPtr<ID3D11SamplerState> sState, Microsoft::WRL::ComPtr<ID3D11Device> dev, const wchar_t* texturePath);
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Camera* camera);

private:
	Mesh* skyboxMesh;
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResource;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
};

