#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Vertex.h"

// For the DirectX Math library
//using namespace DirectX;

class Mesh
{
public:	//Scopes are declared in block form rather than per item
	Mesh(Vertex *vertices, int verticeNum, unsigned int *indices, int indiceNum, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> dContext);	//Constructor
	Mesh(const char* fileName, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> dContext); //3D object constructor
	~Mesh(); //Deconstructor
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
	void CreateBuffers(Vertex* vertices, int verticeNum, unsigned int* indices, int indiceNum, Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> dContext);
	int GetIndexCount();
	void Draw();

private:
	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	//DeviceContext object for draw commands
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;

	//Method not created by me (see above method definition in .cpp file for details)
	void CalculateTangents(Vertex* verts, int numVerts, unsigned int* indices, int numIndices);

	int meshBufferIndices;
};
