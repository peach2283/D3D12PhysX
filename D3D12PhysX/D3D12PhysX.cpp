// D3D12PhysX.cpp: 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include "D3D12PhysX.h"
#include "PhysxLibs.h"
#include "Input.h"
#include "Timer.h"
#include "Device.h"
#include "DebugDraw.h"

#include <stdio.h>

#pragma warning(disable:4996)

#define START_CONSOLE() {AllocConsole();  freopen("CONOUT$", "w", stdout); freopen("CONIN$", "r", stdin);}
#define STOP_CONSOLE()  {FreeConsole();}

HWND InitInstance(HINSTANCE hInstance, int nCmdShow, LPCWSTR CLSName, LPCWSTR APPName, INT width, INT height);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
    using namespace physx;

    MSG msg = { 0 };

    LPCWSTR CLSName = L"D3D12PhysX";
    LPCWSTR APPName = L"다이렉트3D 피직스 물리엔진 테스트 프로그램";

    INT width  = 800;
    INT height = 600;

    HINSTANCE hInst = hInstance;
    HWND hWnd = InitInstance(hInst, nCmdShow, CLSName, APPName, width, height);

    START_CONSOLE();  //// 디버그 콘솔 시작

    printf("프로그램 초기화 ");
    Device::init(hWnd, width, height);
    Input::init(hWnd, width, height);
    Timer::init();
    ////////////////////////////////////디버그..출력객체 생성//////////////////////////////////////
    DebugDraw * debugDraw = new DebugDraw();
    debugDraw->init();

    //////////////////////////////////////피직스 초기화하기//////////////////////////////////////////
    PxDefaultAllocator		Allocator;
    PxDefaultErrorCallback	ErrorCallback;

    PxFoundation*            Foundation = NULL;
    PxPvd*                   Pvd = NULL;
    PxPhysics*	             Physics = NULL;
    PxDefaultCpuDispatcher*	 Dispatcher = NULL;
    PxScene*				 Scene = NULL;
    PxMaterial*				 Material = NULL;

    ///////////////////////////////////피직스 Foundation 생성////////////////////////////////////////
    Foundation = PxCreateFoundation(PX_FOUNDATION_VERSION, Allocator, ErrorCallback);

    if (Foundation == NULL)
    {
        printf("피직스 파운데이션 생성 실패\n");
        return -1;
    }

    ///////////////////////////////PVD(PhysX Visual Debuger 연결 객체 생성/////////////////////////
    Pvd = PxCreatePvd(*Foundation);
    PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("localhost", 5425, 10);

    if (transport == NULL)
    {
        printf("PVD(피직스 비주얼 디버거) 네트워크 연결 객체 생성(소케생성)실패\n");
        return -1;
    }

    if (Pvd->connect(*transport, PxPvdInstrumentationFlag::eDEBUG) != true)
    {
        printf("PVD(피직스 비주얼 디버거)에 연결할 수 없음\n");
    }
    else {
        printf("PVD(피직스 비주얼 디버거)에 연결하였음\n");
    }

    /////////////////////////////피직스 물리시뮬레이션 객체 생성 ////////////////////////////////
    Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *Foundation, PxTolerancesScale(), true, Pvd);

    if (Physics == NULL)
    {
        printf("물리객체(물리엔진) 생성 실패\n");
        return -1;
    }

    ///////////////////////////피직스 씬 생성/////////////////////////////////////////////////
    PxSceneDesc sceneDesc(Physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    Dispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.cpuDispatcher = Dispatcher;
    sceneDesc.filterShader  =  PxDefaultSimulationFilterShader;

    Scene = Physics->createScene(sceneDesc);

    if (Scene == NULL)
    {
        printf("씬(Scene) 생성 실패\n");
        return -1;
    }

    ///////////////////////PVD의 씬객체에..추가설정하기///////////////////////////////////////
    PxPvdSceneClient* pvdClient = Scene->getScenePvdClient();
    if (pvdClient)
    {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }

    //////////////////////충돌체..재질 생성///////////////////////////////////////////////////
    Material = Physics->createMaterial(0.5f, 0.5f, 0.6f);

    //////////////////////Plane(평면) Static Geometry 추가하기///////////////////////////////
    //PxTransform    planeTransform = PxTransform(PxVec3(0, 0, 0), PxQuat(90 * PxPi / 180, PxVec3(0, 0, 1)));
    //PxRigidStatic* plane = PxCreateStatic(*Physics, planeTransform, PxPlaneGeometry(), *Material);
    //Scene->addActor(*plane);
    PxRigidStatic* plane = PxCreatePlane(*Physics, PxPlane(0, 1, 0, 0), *Material);
    Scene->addActor(*plane);

    ////////////////////// Box Static Geometry 추가하기/////////////////////////////////////
    PxTransform    boxTransform = PxTransform(PxVec3(-2, 1, 0), PxQuat(45*PxPi/180, PxVec3(0,1,0)));
    PxRigidStatic* box = PxCreateStatic(*Physics, boxTransform, PxBoxGeometry(1, 1, 1), *Material);
    Scene->addActor(*box);

    ////////////////////// Sphere Static Geometry 추가하기///////////////////////////////////
    PxTransform    sphereTransform = PxTransform(PxVec3(2, 1, 0));
    PxRigidStatic* sphere = PxCreateStatic(*Physics, sphereTransform, PxSphereGeometry(1), *Material);
    Scene->addActor(*sphere);

    ////////////////////// Capsule Static Geometry 추가하기///////////////////////////////////
    PxTransform    capsuleTransform = PxTransform(PxVec3(0, 2, 0), PxQuat(90 * PxPi / 180, PxVec3(0, 0, 1)));
    PxRigidStatic* capsule = PxCreateStatic(*Physics, capsuleTransform, PxCapsuleGeometry(1,1), *Material);
    Scene->addActor(*capsule);

    /////////////////////// Capsule Dynamc 객체 추가하기///////////////////////////////////////
    PxTransform    dnyTransform = PxTransform(PxVec3(0.0f, 2.0f, -3.0f), PxQuat(90 * PxPi / 180, PxVec3(0, 0, 1)));
    PxRigidDynamic* dynamic = PxCreateDynamic(*Physics, dnyTransform, PxCapsuleGeometry(1, 1), *Material, 1.0f);
    //dynamic->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_LINEAR_Z | PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y);
    dynamic->setRigidDynamicLockFlags(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X | PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z);
    Scene->addActor(*dynamic);

    ///////////////////// Capsule Dynamic 객체 이동 및 회전 속도//////////////////////////////////
    PxVec3 linearVelocity (0.0f,        0.0f,        3.0f);  //로칼축 기준 - z축으로 이동
    PxVec3 angularVelocity(90 * PxPi / 180.0f, 0.0f, 3.0f);  //로칼축 기준 - x축으로 회전

    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Device::clear(204, 204, 204);
        Timer::update();
        Input::update();
        
        debugDraw->reset();

        if (Input::getKey(VK_UP) == true || Input::getKey(VK_DOWN) == true)
        {
            PxVec3 velocity = linearVelocity;

            if (Input::getKey(VK_DOWN) == true) velocity = -velocity;

            PxTransform trans=dynamic->getGlobalPose();
            dynamic->setLinearVelocity(trans.rotate(velocity));  //로칼축 기준 이동
        }
        else {

            dynamic->setLinearVelocity(PxVec3(0,0,0));
        }

        if (Input::getKey(VK_LEFT) == true || Input::getKey(VK_RIGHT) == true) {

            PxVec3 velocity = angularVelocity;

            if (Input::getKey(VK_LEFT) == true) velocity = -velocity;

            PxTransform trans = dynamic->getGlobalPose();
            dynamic->setAngularVelocity(trans.rotate(velocity));  //로칼 축 기준 회전
        }
        else {
            dynamic->setAngularVelocity(PxVec3(0, 0, 0));
        }

        ////////////////////////물리 시뮬레이션 하기////////////////////////
        Scene->simulate(Timer::deltaTime);
        Scene->fetchResults(true);

        /////////////[dynamic 캡슐을 기준으로..카메라 트랜스폼 변경/////////
        Camera * cam = debugDraw->getDebugCamera();
        cam->follow(PxVec3(3, 0 , -5), PxVec3(0, -30, -90), dynamic->getGlobalPose());

        //////////디버그 출력하기////////////////////////////////////////////
        debugDraw->draw(Scene);

        Device::render();
    }
 
    ////////////피직스 종료하기//////////
    SafeRelease(Material);
    SafeRelease(Scene);
    SafeRelease(Dispatcher);
    SafeRelease(Physics);
    SafeRelease(transport);
    SafeRelease(Pvd);
    SafeRelease(transport);
    SafeRelease(Foundation);

    SafeDelete(debugDraw);

    Input::exit();
    Timer::exit();
    Device::exit();

    printf("게임 프로그램 종료");
    STOP_CONSOLE();

    return (int)msg.wParam;
}

HWND InitInstance(HINSTANCE hInstance, int nCmdShow, LPCWSTR CLSName, LPCWSTR APPName, INT width, INT height)
{
    HWND       hWnd;
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = CLSName;
    wcex.hIconSm = NULL;

    ///////////////////////////////////////////////
    RECT rt = { 0,0,width, height };
    AdjustWindowRect(&rt, WS_OVERLAPPEDWINDOW, false);


    if (!RegisterClassEx(&wcex)) return NULL;

    hWnd = CreateWindow(CLSName,
                        APPName,
                        WS_OVERLAPPEDWINDOW, //전체모드시 WS_POPUP
                        CW_USEDEFAULT,       //전체모드시 0
                        CW_USEDEFAULT,       //전체모드시 0
                        rt.right - rt.left,
                        rt.bottom - rt.top,
                        NULL,
                        NULL,
                        hInstance,
                        NULL);

    ShowWindow(hWnd, nCmdShow);
    return hWnd;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}