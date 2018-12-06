#pragma once

#include <PxPhysicsAPI.h>
#include <vector>

using namespace physx;

void CreateTetrahedron(std::vector<PxVec3> &data, float size);  // Tetrahedron  [�� 4��ü]
void CreateOctahedron(std::vector<PxVec3> &data, float size);   // Octahedron   [�� 8��ü]
void CreateDodecahedron(std::vector<PxVec3> &data, float size); // Dodecahedron [�� 12 ��ü]
void CreateIcosahedron(std::vector<PxVec3> &data, float size);  // Icosahedron  [�� 20 �nü]
void CreatePyramid(std::vector<PxVec3> &data, float size);      // pyramid      [4����]

PxConvexMesh* CreateConvexMesh(std::vector<PxVec3> &data, PxPhysics* Physics, PxCooking *  Cooking);