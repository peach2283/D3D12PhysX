#pragma once

#include <PxPhysicsAPI.h>
#include <vector>

using namespace physx;

PxHeightFieldSample * readTerrain(const char *filename, int rows, int cols);
