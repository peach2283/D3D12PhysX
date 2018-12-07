#include "stdafx.h"
#include "TriangleMesh.h"

void CreateBoxMesh(float width, float height, float depth, Mesh &mesh)
{
    PxVec3 v[8];

    FLOAT w2 = 0.5f*width;
    FLOAT h2 = 0.5f*height;
    FLOAT d2 = 0.5f*depth;

    // Fill in the top face vertex data.
    v[0] = PxVec3(-w2, +h2, +d2);
    v[1] = PxVec3(+w2, +h2, +d2);
    v[2] = PxVec3(+w2, +h2, -d2);
    v[3] = PxVec3(-w2, +h2, -d2);

    // Fill in the bottom face vertex data.
    v[4] = PxVec3(-w2, -h2, +d2);
    v[5] = PxVec3(+w2, -h2, +d2);
    v[6] = PxVec3(+w2, -h2, -d2);
    v[7] = PxVec3(-w2, -h2, -d2);

    mesh.vtx.assign(&v[0], &v[24]);

    UINT i[36];

    // Fill in the front face index data
    i[0] = 0; i[1] = 1; i[2] = 2;
    i[3] = 0; i[4] = 2; i[5] = 3;

    // Fill in the back face index data
    i[6] = 4; i[7] = 6; i[8] = 5;
    i[9] = 4; i[10] = 7; i[11] = 6;

    // Fill in the top face index data
    i[12] = 0; i[13] = 3; i[14] = 7;
    i[15] = 0; i[16] = 7; i[17] = 4;

    // Fill in the bottom face index data
    i[18] = 1; i[19] = 5; i[20] = 6;
    i[21] = 1; i[22] = 6; i[23] = 2;

    // Fill in the left face index data
    i[24] = 3; i[25] = 2; i[26] = 6;
    i[27] = 3; i[28] = 6; i[29] = 7;

    // Fill in the right face index data
    i[30] = 0; i[31] = 4; i[32] = 5;
    i[33] = 0; i[34] = 5; i[35] = 1;

    mesh.idx.assign(&i[0], &i[36]);
}

PxTriangleMesh* CreateTriangleMesh(Mesh &mesh,PxPhysics* Physics, PxCooking *  Cooking)
{
    PxTriangleMesh *triangleMesh;
   
    PxTriangleMeshDesc meshDesc;
    meshDesc.points.count  = mesh.vtx.size();
    meshDesc.points.stride = sizeof(PxVec3);
    meshDesc.points.data   = &mesh.vtx[0];

    meshDesc.triangles.count = mesh.idx.size() / 3;
    meshDesc.triangles.stride = 3 * sizeof(PxU32);
    meshDesc.triangles.data   = &mesh.idx[0];

    PxDefaultMemoryOutputStream writeBuffer;
    PxTriangleMeshCookingResult::Enum result;
    bool status = Cooking->cookTriangleMesh(meshDesc, writeBuffer, &result);

    if (status != true)
    {
        printf("Triangle Mesh Cooking ½ÇÆÐ\n");
        return NULL;
    }

    PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
    triangleMesh = Physics->createTriangleMesh(readBuffer);

    return triangleMesh;
}

void CreateCylinderMesh(FLOAT bottomRadius, FLOAT topRadius, FLOAT height, UINT sliceCount, UINT stackCount, Mesh& mesh)
{
    void MakeCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, Mesh& mesh);
    void MakeCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, Mesh& mesh);

    mesh.vtx.clear();
    mesh.idx.clear();

    ///////////// Build Stacks.///////////////////////////////////
    float stackHeight = height / stackCount;

    // Amount to increment radius as we move up each stack level from bottom to top.
    float radiusStep = (topRadius - bottomRadius) / stackCount;

    UINT ringCount = stackCount + 1;

    // Compute vertices for each stack ring starting at the bottom and moving up.
    for (UINT i = 0; i < ringCount; ++i)
    {
        float y = -0.5f*height + i * stackHeight;
        float r = bottomRadius + i * radiusStep;

        // vertices of ring
        float dTheta = 2.0f * PxPi / sliceCount;
        for (UINT j = 0; j <= sliceCount; ++j)
        {
            float c = cosf(j*dTheta);
            float s = sinf(j*dTheta);

            mesh.vtx.push_back(PxVec3(r*c, y, r*s));
        }
    }

    // Add one because we duplicate the first and last vertex per ring
    // since the texture coordinates are different.
    UINT ringVertexCount = sliceCount + 1;

    // Compute indices for each stack.
    for (UINT i = 0; i < stackCount; ++i)
    {
        for (UINT j = 0; j < sliceCount; ++j)
        {
            mesh.idx.push_back(i*ringVertexCount + j);
            mesh.idx.push_back((i + 1)*ringVertexCount + j);
            mesh.idx.push_back((i + 1)*ringVertexCount + j + 1);

            mesh.idx.push_back(i*ringVertexCount + j);
            mesh.idx.push_back((i + 1)*ringVertexCount + j + 1);
            mesh.idx.push_back(i*ringVertexCount + j + 1);
        }
    }

    MakeCylinderTopCap(bottomRadius, topRadius, height, sliceCount, stackCount, mesh);
    MakeCylinderBottomCap(bottomRadius, topRadius, height, sliceCount, stackCount, mesh);
}

void MakeCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, Mesh& mesh)
{
    UINT baseIndex = (UINT)mesh.vtx.size();

    float y = 0.5f*height;
    float dTheta = 2.0f*PxPi / sliceCount;

    // Duplicate cap ring vertices because the texture coordinates and normals differ.
    for (UINT i = 0; i <= sliceCount; ++i)
    {
        float x = topRadius * cosf(i*dTheta);
        float z = topRadius * sinf(i*dTheta);

        mesh.vtx.push_back(PxVec3(x, y, z));
    }

    // Cap center vertex.
    mesh.vtx.push_back(PxVec3(0.0f, y, 0.0f));

    // Index of center vertex.
    UINT centerIndex = (UINT)mesh.vtx.size() - 1;

    for (UINT i = 0; i < sliceCount; ++i)
    {
        mesh.idx.push_back(centerIndex);
        mesh.idx.push_back(baseIndex + i + 1);
        mesh.idx.push_back(baseIndex + i);
    }
}

void MakeCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, Mesh &mesh)
{
    UINT baseIndex = (UINT)mesh.vtx.size();
    float y = -0.5f*height;

    // vertices of ring
    float dTheta = 2.0f*PxPi / sliceCount;
    for (UINT i = 0; i <= sliceCount; ++i)
    {
        float x = bottomRadius * cosf(i*dTheta);
        float z = bottomRadius * sinf(i*dTheta);

       mesh.vtx.push_back(PxVec3(x, y, z));
    }

    // Cap center vertex.
    mesh.vtx.push_back(PxVec3(0.0f, y, 0.0f));

    // Cache the index of center vertex.
    UINT centerIndex = (UINT)mesh.vtx.size() - 1;

    for (UINT i = 0; i < sliceCount; ++i)
    {
        mesh.idx.push_back(centerIndex);
        mesh.idx.push_back(baseIndex + i);
        mesh.idx.push_back(baseIndex + i + 1);
    }
}


void CreateTorusMesh(float diameter, float thickness, size_t tessellation, Mesh &mesh)
{
    size_t stride = tessellation + 1;

    // First we loop around the main ring of the torus. 
    for (size_t i = 0; i <= tessellation; i++)
    {
        float u = (float)i / tessellation;

        float outerAngle = i * PxTwoPi / tessellation - PxPiDivTwo;

        // Create a transform matrix that will align geometry to 
        // slice perpendicularly though the current ring position. 
        PxTransform translate = PxTransform(PxVec3(diameter / 2, 0, 0));
        PxTransform rotation = PxTransform(PxQuat(outerAngle, PxVec3(0, 1, 0)));
        PxTransform transform = rotation * translate;

        // Now we loop along the other axis, around the side of the tube. 
        for (size_t j = 0; j <= tessellation; j++)
        {
            float v = 1 - (float)j / tessellation;

            float innerAngle = j * PxTwoPi / tessellation + PxPi;
            float dx, dy;

            dy = PxSin(innerAngle);
            dx = PxCos(innerAngle);

            // Create a vertex. 
            PxVec3 normal(dx, dy, 0);
            PxVec3 position = (thickness / 2) * normal;

            position = transform.transform(position);
            mesh.vtx.push_back(position);

            // And create indices for two triangles. 
            size_t nextI = (i + 1) % stride;
            size_t nextJ = (j + 1) % stride;

            mesh.idx.push_back(i * stride + j);
            mesh.idx.push_back(i * stride + nextJ);
            mesh.idx.push_back(nextI * stride + j);

            mesh.idx.push_back(i * stride + nextJ);
            mesh.idx.push_back(nextI * stride + nextJ);
            mesh.idx.push_back(nextI * stride + j);
        }
    }
}
