#include "stdafx.h"
#include "ConvexPrimitive.h"

static const float SQRT2 = 1.41421356237309504880f;
static const float SQRT3 = 1.73205080756887729352f;
static const float SQRT6 = 2.44948974278317809820f;

// Tetrahedron [4면체]
void CreateTetrahedron( std::vector<PxVec3> &data,  float size)
{
    static const PxVec3 verts[4] =
    {
        { 0.f,                    0.f,                1.f },
        { 2.f*SQRT2 / 3.f,        0.f,               -1.f / 3.f },
        { -SQRT2 / 3.f,          SQRT6 / 3.f,        -1.f / 3.f },
        { -SQRT2 / 3.f,          -SQRT6 / 3.f,       -1.f / 3.f }
    };

    for (int i = 0; i < 4; i++)
    {
        //data.push_back(PxVec3(verts[i].x, verts[i].y, verts[i].z)*size);
        data.push_back( PxVec3(verts[i].x, verts[i].z, -verts[i].y)*size);  //+y를 up 방향으로 회전 시킴
    }
}

// Octahedron [8면체]
void CreateOctahedron(std::vector<PxVec3> &data, float size)
{
    static const PxVec3 verts[6] =
    {
        {  1,  0,  0 },
        { -1,  0,  0 },
        {  0,  1,  0 },
        {  0, -1,  0 },
        {  0,  0,  1 },
        {  0,  0, -1 }
    };

    for (int i = 0; i < 6; i++)
    {
        //data.push_back(PxVec3(verts[i].x, verts[i].y, verts[i].z)*size);
        data.push_back(PxVec3(verts[i].x, verts[i].z, -verts[i].y)*size);  //+y를 up 방향으로 회전 시킴
    }
 }

// Dodecahedron [12 면체]
void CreateDodecahedron(std::vector<PxVec3> &data, float size)
{
    static const float a = 1.f / SQRT3;
    static const float b = 0.356822089773089931942f; // sqrt( ( 3 - sqrt(5) ) / 6 ) 
    static const float c = 0.934172358962715696451f; // sqrt( ( 3 + sqrt(5) ) / 6 ); 

    static const PxVec3 verts[20] =
    {
        { a,  a,  a },
        { a,  a, -a },
        { a, -a,  a },
        {  a, -a, -a },
        { -a,  a,  a },
        { -a,  a, -a },
        { -a, -a,  a },
        { -a, -a, -a },
        { b,  c,  0 },
        { -b,  c,  0 },
        { b, -c,  0 },
        { -b, -c,  0 },
        { c,  0,  b },
        { c,  0, -b },
        { -c,  0,  b },
        { -c,  0, -b },
        { 0,  b,  c },
        { 0, -b,  c },
        { 0,  b, -c },
        { 0, -b, -c }
    };

    for (int i = 0; i < 20; i++)
    {
        //data.push_back(PxVec3(verts[i].x, verts[i].y, verts[i].z)*size);
        data.push_back(PxVec3(verts[i].x, verts[i].z, -verts[i].y)*size);  //+y를 up 방향으로 회전 시킴
    }
}

// Icosahedron [20 몁체]
void CreateIcosahedron(std::vector<PxVec3> &data, float size)
{
 
    static const float  t = 1.618033988749894848205f; // (1 + sqrt(5)) / 2 
    static const float t2 = 1.519544995837552493271f; // sqrt( 1 + sqr( (1 + sqrt(5)) / 2 ) ) 

    static const PxVec3 verts[12] =
    {
        { t / t2,  1.f / t2,       0 },
        { -t / t2,  1.f / t2,       0 },
        { t / t2, -1.f / t2,       0 },
        { -t / t2, -1.f / t2,       0 },
        { 1.f / t2,       0,    t / t2 },
        { 1.f / t2,       0,   -t / t2 },
        { -1.f / t2,       0,    t / t2 },
        { -1.f / t2,       0,   -t / t2 },
        { 0,    t / t2,  1.f / t2 },
        { 0,   -t / t2,  1.f / t2 },
        { 0,    t / t2, -1.f / t2 },
        { 0,   -t / t2, -1.f / t2 }
    };

    for (int i = 0; i < 12; i++)
    {
        //data.push_back(PxVec3(verts[i].x, verts[i].y, verts[i].z)*size);
        data.push_back(PxVec3(verts[i].x, verts[i].z, -verts[i].y)*size);  //+y를 up 방향으로 회전 시킴
    }
}

// pyramid [사각뿔]
void CreatePyramid(std::vector<PxVec3> &data, float size)
{
    static const PxVec3  verts[5] = 
    {   PxVec3(0, 1 ,0),
        PxVec3(1, 0 ,0),
        PxVec3(-1,0, 0),
        PxVec3(0, 0, 1), 
        PxVec3(0, 0,-1) };

    data.push_back(verts[0] * size);
    data.push_back(verts[1] * size);
    data.push_back(verts[2] * size);
    data.push_back(verts[3] * size);
    data.push_back(verts[4] * size);
}

PxConvexMesh* CreateConvexMesh(std::vector<PxVec3> &data, PxPhysics* Physics, PxCooking *  Cooking)
{
    PxConvexMeshDesc convexDesc;
    convexDesc.points.count  = data.size();
    convexDesc.points.stride = sizeof(PxVec3);
    convexDesc.points.data   = &data[0];
    convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

    PxDefaultMemoryOutputStream buf;
    PxConvexMeshCookingResult::Enum result;

    if (Cooking->cookConvexMesh(convexDesc, buf, &result) != true)
    {
        printf("Convex Mesh Cooking 실패\n");
        return NULL;
    }

    PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
    PxConvexMesh* convexMesh = Physics->createConvexMesh(input);

    return convexMesh;
}