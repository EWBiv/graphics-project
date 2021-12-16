#include "Transform.h"

using namespace DirectX;

Transform::Transform()
{
	position = XMFLOAT3(0, 0, 0);
	pitchYawRoll = XMFLOAT3(0, 0, 0);
	scale = XMFLOAT3(1, 1, 1);

	XMMATRIX ident = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, ident);
	XMStoreFloat4x4(&worldInverseTransposeMatrix, ident);

	matricesDirty = false;
}
 
Transform::~Transform()
{

}


DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll()
{
	return pitchYawRoll;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	//check if dirty (needs to be recalculated)
	if(matricesDirty)
		UpdateMatrices();

	return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTranspose()
{
	//check if dirty (needs to be recalculated)
	if(matricesDirty)
		UpdateMatrices();

	return worldInverseTransposeMatrix;
}

// Setters
void Transform::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
	matricesDirty = true;
}
void Transform::SetPitchYawRoll(float pitch, float yaw, float roll)
{
	pitchYawRoll = XMFLOAT3(pitch, yaw, roll);
	matricesDirty = true;
}
void Transform::SetScale(float x, float y, float z)
{
	scale = XMFLOAT3(x, y, z);
	matricesDirty = true;
}

void Transform::MoveRelative(float x, float y, float z)
{
	XMVECTOR rotatedVector = XMVector3Rotate(
		XMVectorSet(x, y, z, 0),
		XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll)));

	XMStoreFloat3(&position, XMLoadFloat3(&position) + rotatedVector);
}

XMFLOAT3 Transform::GetUpVector()
{
	return CreateDirectionVector(0, 1, 0, 0);
}

XMFLOAT3 Transform::GetRightVector()
{
	return CreateDirectionVector(1, 0, 0, 0);
}

XMFLOAT3 Transform::GetForwardVector()
{
	return CreateDirectionVector(0, 0, 1, 0);
}

XMFLOAT3 Transform::CreateDirectionVector(float x, float y, float z, float w)
{
	XMVECTOR localDirection = XMVector3Rotate(
		XMVectorSet(x, y, z, w),
		XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&pitchYawRoll)));

	XMFLOAT3 direction;
	XMStoreFloat3(&direction, localDirection);
	return direction;
}

// 'Transformers'
void Transform::MoveGlobal(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
	matricesDirty = true;
}
void Transform::Rotate(float pitch, float yaw, float roll)
{
	pitchYawRoll.x += pitch;
	pitchYawRoll.y += yaw;
	pitchYawRoll.z += roll;
	matricesDirty = true;
}
void Transform::ScaleBy(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;
	matricesDirty = true;
}



void Transform::UpdateMatrices()
{
	//Actually update the matrices by creating the
	//individual transformation matrices and combining 
	XMMATRIX transMat = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(pitchYawRoll.x,pitchYawRoll.y,pitchYawRoll.z);
	XMMATRIX scaleMat = XMMatrixScaling(scale.x, scale.y, scale.z);

	XMMATRIX worldMat = scaleMat * rotationMat * transMat; //Generally want scale -> rotation -> position (translate) in that order
	XMStoreFloat4x4(&worldMatrix, worldMat);

	//Might as well create inverse transpose matrix too
	XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(worldMat)));

	//We're clean again - UPDATED MATRICES
	matricesDirty = false;
}

