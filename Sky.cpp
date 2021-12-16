#include "Sky.h"

using namespace DirectX;

Sky::Sky(Mesh* mesh, SimpleVertexShader* vShader, SimplePixelShader* pShader, Microsoft::WRL::ComPtr<ID3D11SamplerState> sState, Microsoft::WRL::ComPtr<ID3D11Device> dev, const wchar_t* texturePath)
{
	skyboxMesh = mesh;
	vertexShader = vShader;
	pixelShader = pShader;
	samplerState = sState;

	//Rasterizer State
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	dev.Get()->CreateRasterizerState(&rasterizerDesc, rasterizerState.GetAddressOf());

	//Depth-Stencil State
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	dev.Get()->CreateDepthStencilState(&depthStencilDesc, depthStencilState.GetAddressOf());

	CreateDDSTextureFromFile(dev.Get(), texturePath, nullptr, shaderResource.GetAddressOf());
}

void Sky::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Camera* camera)
{
	//Setting render states
	context->RSSetState(rasterizerState.Get());
	context->OMSetDepthStencilState(depthStencilState.Get(), 0);

	vertexShader->SetShader();
	pixelShader->SetShader();

	//Setting sampler+texture for pixel shader
	pixelShader->SetSamplerState("SkySampler", samplerState);
	pixelShader->SetShaderResourceView("SkyTexture", shaderResource);

	//External data of vertex buffer
	vertexShader->SetMatrix4x4("view", camera->GetView());
	vertexShader->SetMatrix4x4("projection", camera->GetProjection());

	vertexShader->CopyAllBufferData();
	pixelShader->CopyAllBufferData();

	skyboxMesh->Draw();

	//Reseting render states
	context->RSSetState(nullptr);
	context->OMSetDepthStencilState(nullptr,0);

}
