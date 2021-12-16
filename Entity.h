#pragma once
#include "Mesh.h"
#include "Transform.h"
#include "Camera.h"
#include "Material.h"

class Entity
{
private:
	Mesh* mesh_;
	Transform transform_;
	Material* material_;
	bool shouldDraw;
	
public:
	Entity(Mesh* mesh, Material* mat, bool draw);
	~Entity();

	Mesh* GetMesh();
	Transform* GetTransform();
	Material* GetMaterial();

	void SetDrawState(bool draw);
	bool GetDrawState();

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> cntxt, Camera* c,float deltaTime);
};

