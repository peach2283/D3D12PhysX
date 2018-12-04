#include "stdafx.h"
#include "DebugDraw.h"

DebugDraw::DebugDraw()
{
    ///////////디버그용 카메라와 렌더러 포인터///////
    cam    = NULL;
    render = NULL;
    
    boxColor     = PxVec3(0.0f, 0.0f, 1.0f);
    sphereColor  = PxVec3(1.0f, 0.0f, 0.0f);
    capsuleColor = PxVec3(0.0f, 1.0f, 0.0f);

    planeColor   = PxVec3(0.89f, 0.89f, 0.89f);  
    sleepColor   = PxVec3(0.2f, 0.2f, 0.2f);

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
    ///////디버그용 카메라 생성////////
    cam = new Camera(0.0f,  3.0f, -5.0f, //카메라 위치
                     30.0f, 0.0f, 0.0f);  //카메라 회전

    ///////디버그용 렌더러 생성및 초기화////
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
     
        ///////////////////로칼 좌표축 출력하기/////////////////////
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
            printf("eCONVEXMESH\n");
        }
        else if (geoHolder.getType() == PxGeometryType::eTRIANGLEMESH) {
            printf("eTRIANGLEMESH\n");
        }
        else if (geoHolder.getType() == PxGeometryType::eHEIGHTFIELD) {
            printf("eHEIGHTFIELD\n");
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
  
    PxMat33 Q = PxMat33(PX.q);  //회전
    PxVec3  P = PX.p;           //위치 

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

void DebugDraw::drawSphere(PxReal radius, const PxTransform& trans, const PxVec3& color)
{
    PxVec3 center = trans.p;
    PxVec3 up     = trans.q.getBasisVector1();  //y-축 방향 벡터
    PxVec3 axis   = trans.q.getBasisVector0();  //x-축 방향 벡터
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
    //[참고]피직스의 plane은 항상 (1,0,0)을 방향 벡터로 하면..plane의 이동/회전은 transform에 의해서 정해진다

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
