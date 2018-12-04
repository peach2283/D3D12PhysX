#pragma once

#include <DirectXMath.h>
#include <PxPhysicsAPI.h>
#include "Device.h"

using namespace physx;

class Camera
{
private:
    float fov;
    float aspect;
    float nearZ;
    float farZ;

    DirectX::XMFLOAT4 P, R;
public:

    Camera(float tx = 0.0f, float ty = 0.0f, float tz = -2.0f,
        float rx = 0.0f, float ry = 0.0f, float rz = 0.0f);

    ~Camera();

    DirectX::XMMATRIX getView();
    DirectX::XMMATRIX getProj();

    void follow(PxVec3 distance, PxVec3 angle, const PxTransform &ref);
};