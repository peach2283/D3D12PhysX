#include "stdafx.h"
#include "Camera.h"

Camera::Camera(float tx, float ty, float tz, float rx, float ry, float rz)
{
    float pitch = DirectX::XMConvertToRadians(rx);
    float yaw   = DirectX::XMConvertToRadians(ry);
    float roll  = DirectX::XMConvertToRadians(rz);

    DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
    DirectX::XMStoreFloat4(&this->R, Q);

    this->P = DirectX::XMFLOAT4(tx, ty, tz, 1.0f);

    aspect = FLOAT(800) / FLOAT(600);  //ȭ���� ���� ���� ����
    nearZ  = FLOAT(1.0);
    farZ   = FLOAT(1000.0);
    fov    = DirectX::XMConvertToRadians(60);
}

Camera::~Camera()
{
}

DirectX::XMMATRIX Camera::getView()
{
    DirectX::XMVECTOR Q = DirectX::XMLoadFloat4(&R);
    DirectX::XMMATRIX R = DirectX::XMMatrixRotationQuaternion(Q);

    DirectX::XMVECTOR pos = DirectX::XMVectorSet(P.x, P.y, P.z, 1);  //ī�޶� ��Į ����
    // DirectX::XMVECTOR target = DirectX::XMVectorSet(0, 0, 1, 1);  //ī�޶� ��Į �ü� - �޼� ��ǥ��
    DirectX::XMVECTOR target = DirectX::XMVectorSet(0, 0, -1, 1);    //ī�޶� ��Į �ü� - ������ ��ǥ��(PVD)
    DirectX::XMVECTOR     up = DirectX::XMVectorSet(0, 1, 0, 1);     //ī�޶� ��Į ������

    //pos    = DirectX::XMVector3Transform(pos, T);
    target = DirectX::XMVector3Transform(target, R);
    up     = DirectX::XMVector3Transform(up, R);
    target = DirectX::XMVectorAdd(target, pos);

    return DirectX::XMMatrixLookAtLH(pos, target, up);
}

DirectX::XMMATRIX Camera::getProj()
{
    //return DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);  //�޼� ��ǥ��
    return DirectX::XMMatrixPerspectiveFovRH(fov, aspect, nearZ, farZ);    //������ ��ǥ�� (PVD)
}

void Camera::follow(PxVec3 distance, PxVec3 angle, const PxTransform &ref)
{
    PxVec3 pos = ref.transform(distance);

    P.x = pos.x;
    P.y = pos.y;
    P.z = pos.z;

    float  rx = angle.x*PxPi / PxReal(180.0);
    float  ry = angle.y*PxPi / PxReal(180.0);
    float  rz = angle.z*PxPi / PxReal(180.0);

    PxQuat Q = ref.q*PxQuat(rx, PxVec3(1,0,0))*PxQuat(ry, PxVec3(0, 1, 0))*PxQuat(rz, PxVec3(0, 0, 1));

    R.x = Q.x;
    R.y = Q.y;
    R.z = Q.z;
    R.w = Q.w;
}