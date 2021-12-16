#include "Camera.h"
#include "Input.h"

using namespace DirectX;

void Camera::SetFoV(float fov)
{
    fieldOfView = fov;
    UpdateProjectionMatrix(aspectRatio);
}

Camera::Camera(DirectX::XMFLOAT3 pos, float moveSpeed, float lookSpeed, float fov, float aspectRatio, float nearZ, float farZ) 
    : movementSpeed(moveSpeed), mouseLookSpeed(lookSpeed), fieldOfView(fov), aspectRatio(aspectRatio), nearZDistance(nearZ), farZDistance(farZ)
{
    transform.SetPosition(pos.x,pos.y,pos.z);
    UpdateViewMatrix();
    UpdateProjectionMatrix(aspectRatio);
}

Camera::~Camera()
{
}

void Camera::Update(float dt)
{
    //Get a reference to the input manager
    //Input& input = Input::GetInstance();

    //Movement to be moved out of camera at some point for larger project

    //Current speed
    //float speed = movementSpeed * dt;

    //WASD Movement
    //if (input.KeyDown('W')) { transform.MoveRelative(0, 0, speed); }
    //if (input.KeyDown('S')) { transform.MoveRelative(0, 0, -speed); }
    //if (input.KeyDown('A')) { transform.MoveRelative(-speed, 0, 0); }
    //if (input.KeyDown('D')) { transform.MoveRelative(speed, 0, 0); }
    //if (input.KeyDown(' ')) { transform.MoveGlobal(0, speed, 0); }
    //if (input.KeyDown('X')) { transform.MoveGlobal(0, -speed, 0); }

    //Rotation
    //if (input.MouseLeftDown())
    //{
    //    float mouseSpeed = mouseLookSpeed * dt;
    //
    //    float xDiff = mouseSpeed * input.GetMouseXDelta();
    //    float yDiff = mouseSpeed * input.GetMouseYDelta();
    //
    //    //Clamping xDiff value to between -PI/2 & PI/2
    //    XMFLOAT3 rotation = transform.GetPitchYawRoll();
    //    float pitch = rotation.x + xDiff;
    //    if (pitch > XM_PIDIV2) transform.SetPitchYawRoll(XM_PIDIV2, rotation.y, rotation.z);
    //    else if (pitch < -XM_PIDIV2) transform.SetPitchYawRoll(-XM_PIDIV2, rotation.y, rotation.z);
    //
    //    transform.Rotate(yDiff, xDiff, 0);
    //}

    //You can also use controls to adjust FOV here
    

    //At the end, update view
    UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
    XMFLOAT3 pos = transform.GetPosition();
    XMFLOAT3 fwd = transform.GetForwardVector();

    XMMATRIX vMatrix =
       XMMatrixLookToLH(
           XMLoadFloat3(&pos),          //Camera pos
           XMLoadFloat3(&fwd),          //Camera fwd 
           XMVectorSet(0, 1, 0, 0));    //World up (y)

    XMStoreFloat4x4(&viewMatrix, vMatrix);
}


void Camera::UpdateProjectionMatrix(float aspectRatio)
{
    XMMATRIX prjtMatrix = XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearZDistance,farZDistance);
    XMStoreFloat4x4(&projectionMatrix, prjtMatrix);
}

DirectX::XMFLOAT4X4 Camera::GetView()
{
    return viewMatrix;
}

DirectX::XMFLOAT4X4 Camera::GetProjection()
{
    return projectionMatrix;
}

Transform* Camera::GetTransform()
{
    return &transform;
}
