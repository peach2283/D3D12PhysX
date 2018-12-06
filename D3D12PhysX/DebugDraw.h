#pragma once

#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>

#include <PxPhysicsAPI.h>

#include "Camera.h"
#include "DebugRender.h"

using namespace physx;

struct DebugVertex
{
    DirectX::XMFLOAT4 P;  //Point
    DirectX::XMFLOAT4 C;  //color

    DebugVertex(const PxVec3& point, const PxVec3& color)
    {
        P.x = (float)point.x;  P.y = (float)point.y;
        P.z = (float)point.z;  P.w = 1.0f;

        C.x = (float)color.x;  C.y = (float)color.y;
        C.z = (float)color.z;  C.w = 1.0f;
    }
};


class DebugDraw 
{
private:
    
    std::vector<DebugVertex> lines;  //라인목록 저장 STL

    ID3D12Resource * vtxGPUBuffer;   //Vertex Buffer
    ID3D12Resource * vtxUPBuffer;    //Vertex Buffer

    ///////////////디버그 카메라와 렌더러///////////////////////
    Camera      * cam;
    DebugRender * render;

    /////////////////////////////////////////////////////////
    PxVec3 boxColor;
    PxVec3 planeColor;
    PxVec3 sphereColor;
    PxVec3 capsuleColor;
    PxVec3 convexMeshColor;

    PxVec3 sleepColor;

    void drawActor(PxRigidActor * actor);

    /////////////////////PxTransform To DirectX XMMatrix/////
    void PxtoXMMatrix(PxTransform input, DirectX::XMMATRIX* start);

    void drawLine(const PxVec3& from, const PxVec3& to, const PxVec3& color);
    void drawBox(const PxVec3& bbMin, const PxVec3& bbMax, const PxTransform& trans, const PxVec3& color);
    void drawPlane(const PxTransform& transform, const PxVec3& color);
    void drawSpherePatch(const PxVec3& center, const PxVec3& up, const PxVec3& axis, PxReal radius, PxReal minTh, PxReal maxTh, PxReal minPs, PxReal maxPs, const PxVec3& color, PxReal stepDegrees = 10.f, bool drawCenter = true);
    void drawSphere(PxReal radius, const PxTransform& trans, const PxVec3& color);
    void drawTransform(const PxTransform& trans, PxReal orthoLen);
    void drawCapsule(PxReal radius, PxReal halfHeight, const PxTransform& trans, const PxVec3& color);
    void drawPolygon(const PxVec3* vtxBuffer, const PxU8 *idxBuffer, int nvtx, const PxTransform& trans, const PxVec3& color);

public:

    DebugDraw();
    virtual ~DebugDraw();

    void reset();
  
    void init();
    void draw(PxScene *scene);

    Camera  * getDebugCamera();
};