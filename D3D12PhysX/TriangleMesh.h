#pragma once

#include <PxPhysicsAPI.h>
#include <vector>

using namespace physx;

struct Mesh
{
    std::vector<PxVec3> vtx;
    std::vector<PxU32> idx;
};

void CreateTorusMesh(float diameter, float thickness, size_t tessellation, Mesh &mesh);
void CreateBoxMesh(float width, float height, float depth, Mesh &mesh);
void CreateCylinderMesh(FLOAT bottomRadius, FLOAT topRadius, FLOAT height, UINT sliceCount, UINT stackCount, Mesh& mesh);

PxTriangleMesh* CreateTriangleMesh(Mesh &mesh , PxPhysics* Physics, PxCooking *  Cooking);
