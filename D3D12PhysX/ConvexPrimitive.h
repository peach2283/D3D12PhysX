#pragma once

#include <PxPhysicsAPI.h>
#include <vector>

using namespace physx;

void CreateTetrahedron(std::vector<PxVec3> &data, float size);  // Tetrahedron  [정 4면체]
void CreateOctahedron(std::vector<PxVec3> &data, float size);   // Octahedron   [정 8면체]
void CreateDodecahedron(std::vector<PxVec3> &data, float size); // Dodecahedron [정 12 면체]
void CreateIcosahedron(std::vector<PxVec3> &data, float size);  // Icosahedron  [정 20 몁체]
void CreatePyramid(std::vector<PxVec3> &data, float size);      // pyramid      [4각뿔]

PxConvexMesh* CreateConvexMesh(std::vector<PxVec3> &data, PxPhysics* Physics, PxCooking *  Cooking);