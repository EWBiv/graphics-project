#pragma once

#include <DirectXMath.h>
#include <vector>

class Transform
{
public:
	Transform();
	~Transform();
		
	// Getters
	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll();
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTranspose();
	DirectX::XMFLOAT3 GetUpVector();
	DirectX::XMFLOAT3 GetRightVector();
	DirectX::XMFLOAT3 GetForwardVector();

	// Setters
	void SetPosition(float x, float y, float z);
	void SetPitchYawRoll(float pitch, float yaw, float roll);
	void SetScale(float x, float y, float z);
 
	

	// 'Delta Setters' (add to existing values)
	void MoveRelative(float x, float y, float z);
	void MoveGlobal(float x, float y, float z);
	void Rotate(float pitch, float yaw, float roll);
	void ScaleBy(float x, float y, float z);



	//Hierarchy stuff - just need a simple implementation
	void AddChild(Transform* transform);
	void SetParent(Transform* transform);
	

private:
	//Stuff used for Hierarchy
	//Have pointer to parent + list of pointesr to all children
	//Inspired by Chris Cascioli's code for Transform Hierarchies https://github.com/vixorien/ggp-demos/tree/main/05%20-%20Transform%20Hierarchies
	Transform* parent;
	std::vector<Transform*> children;
	
	//Raw transformation data
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 pitchYawRoll;

	//Matrices
	bool matricesDirty;
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;

	//Helper function for GetDirectionVectors that are public
	DirectX::XMFLOAT3 CreateDirectionVector(float x, float y, float z, float w);

	// Helper for updating matrices
	void UpdateMatrices();
};

