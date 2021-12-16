#include "Entity.h"
#include <d3d11.h>

using namespace DirectX;

Entity::Entity(Mesh* mesh, Material* mat, bool draw)
{
	mesh_ = mesh;
	material_ = mat;

	shouldDraw = draw;
}

Entity::~Entity()
{
}

Mesh* Entity::GetMesh()
{
	return mesh_;
}

Transform* Entity::GetTransform()
{
	return &transform_;
}

Material* Entity::GetMaterial()
{
	return material_;
}

void Entity::SetDrawState(bool draw)
{
	shouldDraw = draw;
}

bool Entity::GetDrawState()
{
	return shouldDraw;
}

void Entity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> cntxt,Camera* c,float totalTime)
{
	//Used to have to do VS or PS SetShader from the context before added simple shader
	material_->GetVertexShader()->SetShader();
	material_->GetPixelShader()->SetShader();
	
	SimpleVertexShader* vs = material_->GetVertexShader();
	vs->SetMatrix4x4("world", transform_.GetWorldMatrix());	//Get world matrix from entities transform
	vs->SetMatrix4x4("view", c->GetView());					//Get View matrix from passed in camera
	vs->SetMatrix4x4("projection", c->GetProjection());			//Get Projection matrix from passed in camera
	vs->SetMatrix4x4("worldInvTranspose", transform_.GetWorldInverseTranspose());

	vs->CopyAllBufferData();
	//Used to have to manually copy it over using memcpy, but now uses simple shader call to copy the buffer 

	SimplePixelShader* ps = material_->GetPixelShader();
	ps->SetFloat4("colorTint", material_->GetColorTint());	//Get color tint from the material
	ps->SetFloat3("cameraPosition", c->GetTransform()->GetPosition()); //Needs to be adjusted when >1 camera is in scene - needs current camera

	ps->CopyAllBufferData();

	mesh_->Draw();
}
