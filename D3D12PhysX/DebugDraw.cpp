#include "stdafx.h"
#include "DebugDraw.h"

DebugDraw::DebugDraw()
{
    ///////////����׿� ī�޶�� ������ ������///////
    cam    = NULL;
    render = NULL;
    
    boxColor          = PxVec3(0.0f, 0.0f, 1.0f);
    sphereColor       = PxVec3(1.0f, 0.0f, 0.0f);
    capsuleColor      = PxVec3(0.0f, 1.0f, 0.0f);
    convexMeshColor   = PxVec3(1.0f, 0.0f, 1.0f);
    triangleMeshColor = PxVec3(0.0f, 1.0f, 1.0f);
    heightFieldColor  = PxVec3(0.89f, 0.89f, 0.89f);
    planeColor      = PxVec3(0.89f, 0.89f, 0.89f);  
    sleepColor      = PxVec3(0.2f, 0.2f, 0.2f);

    vtxGPUBuffer = NULL;
    vtxUPBuffer  = NULL;
}

DebugDraw::~DebugDraw()
{
    SafeDelete(cam);
    SafeDelete(render);
}

void DebugDraw::init()
{
    ///////����׿� ī�޶� ����////////
    cam = new Camera(0.0f,  30.0f, -70.5f, //ī�޶� ��ġ
                     30.0f, 0.0f, 0.0f);  //ī�޶� ȸ��

    ///////����׿� ������ ������ �ʱ�ȭ////
    render = new DebugRender();
    render->init();
}

void DebugDraw::reset()
{
    ReleaseCOM(vtxGPUBuffer);
    ReleaseCOM(vtxUPBuffer);
  
    lines.clear();
}

void DebugDraw::drawActor(PxRigidActor * actor)
{
    PxU32 num = actor->getNbShapes();

    std::vector<PxShape *> shapes(num); 
    actor->getShapes(&shapes[0], num);

    bool sleeping = actor->is<PxRigidDynamic>() ? actor->is<PxRigidDynamic>()->isSleeping() : false;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    for (PxU32 idx = 0; idx < num ; idx++)
    {
        PxTransform transform      = PxShapeExt::getGlobalPose(*shapes[idx], *actor);
        PxGeometryHolder geoHolder = shapes[idx]->getGeometry();
     
        ///////////////////��Į ��ǥ�� ����ϱ�/////////////////////
        drawTransform(transform, 0.5f);

        if (geoHolder.getType() == PxGeometryType::eBOX)
        {
            PxBoxGeometry box;
            PxVec3 color;

            shapes[idx]->getBoxGeometry(box);
           
            PxVec3 BBMin(-box.halfExtents.x, -box.halfExtents.y, -box.halfExtents.z);
            PxVec3 BBMax(+box.halfExtents.x, +box.halfExtents.y, +box.halfExtents.z);

            color = (sleeping == true) ? sleepColor : boxColor;
            drawBox(BBMin, BBMax, transform, color);
        }
        else if (geoHolder.getType() == PxGeometryType::eSPHERE) {
           
            PxSphereGeometry sphere;
            PxVec3 color;
           
            shapes[idx]->getSphereGeometry(sphere);

            color = (sleeping == true) ? sleepColor : sphereColor;
            drawSphere(sphere.radius, transform, color);

        }
        else if (geoHolder.getType() == PxGeometryType::eCAPSULE) {

            PxCapsuleGeometry capsule;
            PxVec3 color;

            shapes[idx]->getCapsuleGeometry(capsule);

            color = (sleeping == true) ? sleepColor : capsuleColor;
            drawCapsule(capsule.radius, capsule.halfHeight, transform, color);
        }
        else if (geoHolder.getType() == PxGeometryType::eCONVEXMESH) {

            PxConvexMeshGeometry convex;         
            PxVec3  color;

            const PxU8   * idxBuffer;
            const PxVec3 * vtxBuffer;

            color = (sleeping == true) ? sleepColor : convexMeshColor;

            shapes[idx]->getConvexMeshGeometry(convex);
         
            idxBuffer = convex.convexMesh->getIndexBuffer();
            vtxBuffer = convex.convexMesh->getVertices();

            for (PxU32 i = 0; i < convex.convexMesh->getNbPolygons(); i++)
            {
                PxHullPolygon        data;
                convex.convexMesh->getPolygonData(i, data);

                drawPolygon(vtxBuffer, &idxBuffer[data.mIndexBase], data.mNbVerts, transform, color);
           }
        }
        else if (geoHolder.getType() == PxGeometryType::eTRIANGLEMESH) {

            PxTriangleMeshGeometry tri;
            PxVec3  color;
            bool    is16Bit;

            const void   * idxBuffer;
            const PxVec3 * vtxBuffer;

            color = (sleeping == true) ? sleepColor : triangleMeshColor;

            shapes[idx]->getTriangleMeshGeometry(tri);

            vtxBuffer = tri.triangleMesh->getVertices();
            idxBuffer = tri.triangleMesh->getTriangles();
            is16Bit   = tri.triangleMesh->getTriangleMeshFlags() & PxTriangleMeshFlag::e16_BIT_INDICES;

            for (PxU32 i = 0; i < tri.triangleMesh->getNbTriangles(); i++)
            {
                PxVec3 v0, v1, v2;
                PxU32  i0, i1, i2;

                if (is16Bit != 0)
                {
                    i0 = ((PxU16*)idxBuffer)[i * 3 + 0];
                    i1 = ((PxU16*)idxBuffer)[i * 3 + 1];
                    i2 = ((PxU16*)idxBuffer)[i * 3 + 2];
                }
                else {
                    i0 = ((PxU32*)idxBuffer)[i * 3 + 0];
                    i1 = ((PxU32*)idxBuffer)[i * 3 + 1];
                    i2 = ((PxU32*)idxBuffer)[i * 3 + 2];
                }

                v0 = vtxBuffer[i0];
                v1 = vtxBuffer[i1];
                v2 = vtxBuffer[i2];

                drawTriangle(v0, v1, v2, transform, color);
            }

        }
        else if (geoHolder.getType() == PxGeometryType::eHEIGHTFIELD) {

            PxHeightFieldGeometry heightFieldGeom;
            PxHeightField* heightField;

            shapes[idx]->getHeightFieldGeometry(heightFieldGeom);
            heightField = heightFieldGeom.heightField;

            int cols = heightField->getNbColumns();
            int rows = heightField->getNbRows();

            float heightScale = heightFieldGeom.heightScale;
            float rowScale    = heightFieldGeom.rowScale;
            float colScale    = heightFieldGeom.columnScale;

            for (int c = 0; c < cols - 1; c++)
            {
                for (int r = 0; r < rows - 1; r++)
                {
                    float x0 = (r + 0)* rowScale;
                    float x1 = (r + 1)* rowScale;

                    float z0 = (c + 0)* colScale;
                    float z1 = (c + 1)* colScale;

                    float y00 = heightField->getHeight(r + 0.0f, c + 0.0f) * heightScale;
                    float y10 = heightField->getHeight(r + 1.0f, c + 0.0f) * heightScale;
                    float y01 = heightField->getHeight(r + 0.0f, c + 1.0f) * heightScale;

                    drawTriangle(PxVec3(x0, y00, z0), PxVec3(x1, y10, z0), PxVec3(x0, y01, z1), transform, heightFieldColor);

                    if (r == rows - 2) 
                    {
                        float y11 = heightField->getHeight(r + 1.0f, c + 1.0f) * heightScale;

                        PxVec3 from=transform.transform(PxVec3(x1, y10, z0));
                        PxVec3 to  =transform.transform(PxVec3(x1, y11, z1));

                        drawLine(from, to, heightFieldColor);
                    }

                    if (c == cols - 2)
                    {
                        float y11 = heightField->getHeight(r + 1.0f, c + 1.0f) * heightScale;

                        PxVec3 from = transform.transform(PxVec3(x0, y01, z1));
                        PxVec3 to   = transform.transform(PxVec3(x1, y11, z1));

                        drawLine(from, to, heightFieldColor);
                    }

                   /****************************************************************
                    float z00 = (c +0)* colScale;
                    float x00 = (r +0)* rowScale;
                    float y00 = heightField->getHeight(r+0, c+0) * heightScale;

                    float z10 = (c+0) * colScale;
                    float x10 = (r+1) * rowScale;
                    float y10 = heightField->getHeight(r+1, c+0) * heightScale;

                    float z01 = (c + 1) * colScale;
                    float x01 = (r + 0) * rowScale;
                    float y01 = heightField->getHeight(r + 0, c+1) * heightScale;

                    float z11 = (c+1) * colScale;
                    float x11 = (r+1) * rowScale;
                    float y11 = heightField->getHeight(r+1, c+1) * heightScale;

                    drawTriangle(PxVec3(x00, y00, z00), PxVec3(x10, y10, z10), PxVec3(x01, y01, z01), transform, heightFieldColor);
                   // drawTriangle(PxVec3(x00, y00, z00), PxVec3(x10, y10, z10), PxVec3(x01, y01, z01), transform, heightFieldColor);
                   *************************************************************/
                }
            }

        }
        else if (geoHolder.getType() == PxGeometryType::ePLANE) {
           
            PxVec3 color;
            //PxPlaneGeometry plane;
            //shapes[idx]->getPlaneGeometry(plane);
            
            color = (sleeping == true) ? sleepColor : planeColor;
            drawPlane(transform, color);
        }
        else if (geoHolder.getType() == PxGeometryType::eGEOMETRY_COUNT) {
            printf("eGEOMETRY_COUNT\n");
        }
        else if (geoHolder.getType() == PxGeometryType::eINVALID) {
            printf("eINVALID\n");
        }
    }

    ////////////////////////////////
    shapes.clear();
}

void DebugDraw::draw(PxScene *scene)
{
    PxActorTypeFlags actorFlags = PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC;

    PxU32 num = scene->getNbActors(actorFlags);

    if (num > 0)
    {
        std::vector<PxRigidActor*> actors(num);
        scene->getActors(actorFlags, (PxActor**)(&actors[0]), num);

        for (PxU32 idx = 0; idx <  actors.size(); idx++)
        {
            drawActor(actors[idx]);
        }   

        actors.clear();

        if (render != NULL && cam != NULL)
        {
            vtxGPUBuffer = Device::createBuffer(&lines[0], lines.size() * sizeof(DebugVertex), &vtxUPBuffer);

            D3D12_VERTEX_BUFFER_VIEW vbv;
            vbv.BufferLocation = vtxGPUBuffer->GetGPUVirtualAddress();
            vbv.StrideInBytes  = sizeof(DebugVertex);
            vbv.SizeInBytes    = sizeof(DebugVertex)*lines.size();

            render->draw(&vbv, lines.size(), &cam->getView(), &cam->getProj());
        }
    }
}

Camera * DebugDraw::getDebugCamera()
{
    return cam;
}

void DebugDraw::PxtoXMMatrix(PxTransform PX, DirectX::XMMATRIX* M)
{
  
    PxMat33 Q = PxMat33(PX.q);  //ȸ��
    PxVec3  P = PX.p;           //��ġ 

   M->r[0] = DirectX::XMVectorSet(Q.column0[0], Q.column0[1], Q.column0[2], 0.0f);
   M->r[1] = DirectX::XMVectorSet(Q.column1[0], Q.column1[1], Q.column1[2], 0.0f);
   M->r[2] = DirectX::XMVectorSet(Q.column2[0], Q.column2[1], Q.column2[2], 0.0f);
   M->r[3] = DirectX::XMVectorSet(P.x, P.y, P.z, 1.0f);
}

void DebugDraw::drawLine(const PxVec3& from, const PxVec3& to, const PxVec3& color)
{
    lines.push_back(DebugVertex(from, color));
    lines.push_back(DebugVertex(to, color));
}

void DebugDraw::drawTriangle(PxVec3 v0, PxVec3 v1, PxVec3 v2, const PxTransform& trans, const PxVec3& color)
{
    v0 = trans.transform(v0);
    v1 = trans.transform(v1);
    v2 = trans.transform(v2);

    drawLine(v0, v1, color);
    drawLine(v1, v2, color);
    drawLine(v2, v0, color);
}

void  DebugDraw::drawPolygon(const PxVec3* vtxBuffer, const PxU8 *idxBuffer, int nvtx, const PxTransform& trans, const PxVec3& color)
{
    PxVec3 vtx1;
    PxVec3 vtx0 = vtxBuffer[idxBuffer[nvtx - 1]];

    vtx0 = trans.transform(vtx0);

    for (int v = 0; v < nvtx; v++)
    {
        vtx1 = vtxBuffer[idxBuffer[v]];
        vtx1 = trans.transform(vtx1);

        drawLine(vtx0, vtx1, color);

        vtx0 = vtx1;
    }
}

void DebugDraw::drawSphere(PxReal radius, const PxTransform& trans, const PxVec3& color)
{
    PxVec3 center = trans.p;
    PxVec3 up     = trans.q.getBasisVector1();  //y-�� ���� ����
    PxVec3 axis   = trans.q.getBasisVector0();  //x-�� ���� ����
    PxReal minTh = -PxHalfPi;
    PxReal maxTh =  PxHalfPi;
    PxReal minPs = -PxHalfPi;
    PxReal maxPs =  PxHalfPi;
    PxReal stepDegrees = 30.f;
    drawSpherePatch(center, up, axis, radius, minTh, maxTh, minPs, maxPs, color, stepDegrees, false);
    drawSpherePatch(center, up, -axis, radius, minTh, maxTh, minPs, maxPs, color, stepDegrees, false);
}

void  DebugDraw::drawBox(const PxVec3& bbMin, const PxVec3& bbMax, const PxTransform& trans, const PxVec3& color)
{
    drawLine( trans.transform(PxVec3(bbMin[0], bbMin[1], bbMin[2])), trans.transform(PxVec3(bbMax[0], bbMin[1], bbMin[2])), color);
    drawLine( trans.transform(PxVec3(bbMax[0], bbMin[1], bbMin[2])), trans.transform(PxVec3(bbMax[0], bbMax[1], bbMin[2])), color);
    drawLine( trans.transform(PxVec3(bbMax[0], bbMax[1], bbMin[2])), trans.transform(PxVec3(bbMin[0], bbMax[1], bbMin[2])), color);
    drawLine( trans.transform(PxVec3(bbMin[0], bbMax[1], bbMin[2])), trans.transform(PxVec3(bbMin[0], bbMin[1], bbMin[2])), color);
    drawLine( trans.transform(PxVec3(bbMin[0], bbMin[1], bbMin[2])), trans.transform(PxVec3(bbMin[0], bbMin[1], bbMax[2])), color);
    drawLine( trans.transform(PxVec3(bbMax[0], bbMin[1], bbMin[2])), trans.transform(PxVec3(bbMax[0], bbMin[1], bbMax[2])), color);
    drawLine( trans.transform(PxVec3(bbMax[0], bbMax[1], bbMin[2])), trans.transform(PxVec3(bbMax[0], bbMax[1], bbMax[2])), color);
    drawLine( trans.transform(PxVec3(bbMin[0], bbMax[1], bbMin[2])), trans.transform(PxVec3(bbMin[0], bbMax[1], bbMax[2])), color);
    drawLine( trans.transform(PxVec3(bbMin[0], bbMin[1], bbMax[2])), trans.transform(PxVec3(bbMax[0], bbMin[1], bbMax[2])), color);
    drawLine( trans.transform(PxVec3(bbMax[0], bbMin[1], bbMax[2])), trans.transform(PxVec3(bbMax[0], bbMax[1], bbMax[2])), color);
    drawLine( trans.transform(PxVec3(bbMax[0], bbMax[1], bbMax[2])), trans.transform(PxVec3(bbMin[0], bbMax[1], bbMax[2])), color);
    drawLine( trans.transform(PxVec3(bbMin[0], bbMax[1], bbMax[2])), trans.transform(PxVec3(bbMin[0], bbMin[1], bbMax[2])), color);
}

void DebugDraw::drawPlane(const PxTransform& trans, const PxVec3& color)
{
    //[����]�������� plane�� �׻� (1,0,0)�� ���� ���ͷ� �ϸ�..plane�� �̵�/ȸ���� transform�� ���ؼ� ��������

    PxVec3 yaxis(0,1,0), zaxis(0,0,1);
    int    gridNum  = 30;
    PxReal lineSize = 10.f;
    PxReal gridLen  = 2 * lineSize / gridNum;

    for (int gridIdx = 0; gridIdx <= gridNum; gridIdx++)
    {
        PxVec3 voffset = zaxis * gridLen*(gridNum / 2.0f - gridIdx);

        PxVec3 pt0 =  yaxis * lineSize + voffset;
        PxVec3 pt1 = -yaxis * lineSize + voffset;

        drawLine(trans.transform(pt0), trans.transform(pt1), color);
    }

    for (int gridIdx = 0; gridIdx <= gridNum; gridIdx++)
    {
        PxVec3 hoffset = yaxis * gridLen*(gridNum / 2.0f - gridIdx);

        PxVec3 pt0 =  zaxis * lineSize + hoffset;
        PxVec3 pt1 = -zaxis * lineSize + hoffset;

        drawLine(trans.transform(pt0), trans.transform(pt1), color);
    }
}

void DebugDraw::drawTransform(const PxTransform& trans, PxReal orthoLen)
{
    PxVec3 start = trans.p;
    drawLine(start, start + trans.q.rotate(PxVec3(orthoLen, 0, 0)), PxVec3(1.0f, 0.3f, 0.3f));
    drawLine(start, start + trans.q.rotate(PxVec3(0, orthoLen, 0)), PxVec3(0.3f, 1.0f, 0.3f));
    drawLine(start, start + trans.q.rotate(PxVec3(0, 0, orthoLen)), PxVec3(0.3f, 0.3f, 1.0f));
}

void DebugDraw::drawCapsule(PxReal radius, PxReal halfHeight, const PxTransform& trans, const PxVec3& color)
{
    int stepDegrees = 30;

    PxVec3 capStart(0.f, 0.f, 0.f);
    capStart[0] = -halfHeight;

    PxVec3 capEnd(0.f, 0.f, 0.f);
    capEnd[0] = halfHeight;

    // Draw the ends
    {
        PxTransform childTransform = trans;
        childTransform.p = trans.transform (capStart);
        {
            PxVec3 center = childTransform.p;
            PxVec3 up     = childTransform.q.getBasisVector1();
            PxVec3 axis   = -childTransform.q.getBasisVector0();
            PxReal minTh = -PxHalfPi;
            PxReal maxTh =  PxHalfPi;
            PxReal minPs = -PxHalfPi;
            PxReal maxPs =  PxHalfPi;

            drawSpherePatch(center, up, axis, radius, minTh, maxTh, minPs, maxPs, color, PxReal(stepDegrees), false);
        }
    }

    {
        PxTransform childTransform = trans;
        childTransform.p  = trans.transform(capEnd);
        {
            PxVec3 center = childTransform.p;
            PxVec3 up = childTransform.q.getBasisVector1();
            PxVec3 axis = childTransform.q.getBasisVector0();
            PxReal minTh = -PxHalfPi;
            PxReal maxTh = PxHalfPi;
            PxReal minPs = -PxHalfPi;
            PxReal maxPs = PxHalfPi;
            drawSpherePatch(center, up, axis, radius, minTh, maxTh, minPs, maxPs, color, PxReal(stepDegrees), false);
        }
    }

    // Draw some additional lines
    PxVec3 start = trans.p;

    for (int i = 0; i<360; i += stepDegrees)
    {
        capEnd[1] = capStart[1] = PxSin(PxReal(i)* PxTwoPi / PxReal(360.0))*radius;
        capEnd[2] = capStart[2] = PxCos(PxReal(i)* PxTwoPi / PxReal(360.0))*radius;
        drawLine(start + trans.q.rotate( capStart), start + trans.q.rotate(capEnd), color);
    }
}

void DebugDraw::drawSpherePatch(const PxVec3& center, const PxVec3& up, const PxVec3& axis, PxReal radius,PxReal minTh, PxReal maxTh, PxReal minPs, PxReal maxPs, const PxVec3& color, PxReal stepDegrees, bool drawCenter)
{
    PxVec3 vA[74];
    PxVec3 vB[74];
    PxVec3 *pvA = vA, *pvB = vB, *pT;
    PxVec3 npole = center + up * radius;
    PxVec3 spole = center - up * radius;
    PxVec3 arcStart;
    PxReal step = stepDegrees * PxTwoPi/PxReal(360.0);
    const PxVec3 & kv = up;
    const PxVec3 & iv = axis;
    
    PxVec3 jv = kv.cross(iv);

    bool drawN = false;
    bool drawS = false;
    if (minTh <= -PxHalfPi)
    {
        minTh = -PxHalfPi + step;
        drawN = true;
    }
    if (maxTh >= PxHalfPi)
    {
        maxTh = PxHalfPi - step;
        drawS = true;
    }
    if (minTh > maxTh)
    {
        minTh = -PxHalfPi + step;
        maxTh =  PxHalfPi - step;
        drawN = drawS = true;
    }
    int n_hor = (int)((maxTh - minTh) / step) + 1;
    if (n_hor < 2) n_hor = 2;
    PxReal step_h = (maxTh - minTh) / PxReal(n_hor - 1);
    bool isClosed = false;
    if (minPs > maxPs)
    {
        minPs = -PxPi + step;
        maxPs = PxPi;
        isClosed = true;
    }
    else if ((maxPs - minPs) >= PxPi * PxReal(2.f))
    {
        isClosed = true;
    }
    else
    {
        isClosed = false;
    }
    int n_vert = (int)((maxPs - minPs) / step) + 1;
    if (n_vert < 2) n_vert = 2;
    PxReal step_v = (maxPs - minPs) / PxReal(n_vert - 1);
    for (int i = 0; i < n_hor; i++)
    {
        PxReal th = minTh + PxReal(i) * step_h;
        PxReal sth = radius * PxSin(th);
        PxReal cth = radius * PxCos(th);
        for (int j = 0; j < n_vert; j++)
        {
            PxReal psi = minPs + PxReal(j) * step_v;
            PxReal sps = PxSin(psi);
            PxReal cps = PxCos(psi);
            pvB[j] = center + cth * cps * iv + cth * sps * jv + sth * kv;
            if (i)
            {
                drawLine(pvA[j], pvB[j], color);
            }
            else if (drawS)
            {
                drawLine(spole, pvB[j], color);
            }
            if (j)
            {
                drawLine(pvB[j - 1], pvB[j], color);
            }
            else
            {
                arcStart = pvB[j];
            }
            if ((i == (n_hor - 1)) && drawN)
            {
                drawLine(npole, pvB[j], color);
            }

            if (drawCenter)
            {
                if (isClosed)
                {
                    if (j == (n_vert - 1))
                    {
                        drawLine(arcStart, pvB[j], color);
                    }
                }
                else
                {
                    if (((!i) || (i == (n_hor - 1))) && ((!j) || (j == (n_vert - 1))))
                    {
                        drawLine(center, pvB[j], color);
                    }
                }
            }
        }
        pT = pvA; pvA = pvB; pvB = pT;
    }
}
