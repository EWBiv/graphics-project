#pragma once

#include "Transform.h"

class Camera
{
public:
	Camera(DirectX::XMFLOAT3 position, float moveSpeed, float lookSpeed, float fov, float aspectRatio, float nearZ, float farZ);
	~Camera();

	//Update
	void Update(float dt);
	void UpdateViewMatrix();

	void UpdateProjectionMatrix(float aspectRatio);

	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();
	Transform* GetTransform();

	void SetFoV(float fov);

private:
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	//Since the camera needs a way to store its place in space
	Transform transform;

	float movementSpeed;
	float mouseLookSpeed;

	float fieldOfView;
	float aspectRatio;
	float nearZDistance;
	float farZDistance;
};

