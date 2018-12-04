#pragma once

#include <PxPhysicsAPI.h>

#define SafeRelease(x) { if(x){ x->release(); x = NULL; } }

#if defined(_WIN32) && !defined(_WIN64)

    #if defined(_DEBUG)

        ////////////////////////////x86-디버그 모드 라이브러리 /////////////////////////////
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win32/debug/PsFastXmlDEBUG_x86.lib")
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win32/debug/PxFoundationDEBUG_x86.lib")
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win32/debug/PxPvdSDKDEBUG_x86.lib")
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win32/debug/PxTaskDEBUG_x86.lib")

        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/debug/LowLevelDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/debug/LowLevelAABBDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/debug/LowLevelClothDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/debug/LowLevelDynamicsDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/debug/LowLevelParticlesDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/debug/PhysX3ExtensionsDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/debug/PhysX3VehicleDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/debug/SceneQueryDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/debug/SimulationControllerDEBUG.lib")

        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/debug/PhysX3CharacterKinematicDEBUG_x86.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/debug/PhysX3CommonDEBUG_x86.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/debug/PhysX3CookingDEBUG_x86.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/debug/PhysX3DEBUG_x86.lib")

    #else

        ////////////////////////////x86-릴리즈 모드 라이브러리 /////////////////////////////
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win32/release/PsFastXml_x86.lib")
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win32/release/PxFoundation_x86.lib")
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win32/release/PxPvdSDK_x86.lib")
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win32/release/PxTask_x86.lib")

        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/release/LowLevel.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/release/LowLevelAABB.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/release/LowLevelCloth.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/release/LowLevelDynamics.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/release/LowLevelParticles.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/release/PhysX3Extensions.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/release/PhysX3Vehicle.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/release/SceneQuery.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/release/SimulationController.lib")

        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/release/PhysX3CharacterKinematic_x86.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/release/PhysX3Common_x86.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/release/PhysX3Cooking_x86.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win32/release/PhysX3_x86.lib")

    #endif

#elif defined(_WIN32) && defined(_WIN64)

    #if defined(_DEBUG )

        ////////////////////////////x64-디버그 모드 라이브러리 /////////////////////////////
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win64/debug/PsFastXmlDEBUG_x64.lib")
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win64/debug/PxFoundationDEBUG_x64.lib")
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win64/debug/PxPvdSDKDEBUG_x64.lib")
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win64/debug/PxTaskDEBUG_x64.lib")

        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/debug/LowLevelDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/debug/LowLevelAABBDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/debug/LowLevelClothDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/debug/LowLevelDynamicsDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/debug/LowLevelParticlesDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/debug/PhysX3ExtensionsDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/debug/PhysX3VehicleDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/debug/SceneQueryDEBUG.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/debug/SimulationControllerDEBUG.lib")

        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/debug/PhysX3CharacterKinematicDEBUG_x64.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/debug/PhysX3CommonDEBUG_x64.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/debug/PhysX3CookingDEBUG_x64.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/debug/PhysX3DEBUG_x64.lib")

    #else

        ////////////////////////////x64-릴리즈 모드 라이브러리 /////////////////////////////
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win64/release/PsFastXml_x64.lib")
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win64/release/PxFoundation_x64.lib")
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win64/release/PxPvdSDK_x64.lib")
        #pragma comment(lib,"PhysX/PxShared/lib/vc15win64/release/PxTask_x64.lib")

        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/release/LowLevel.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/release/LowLevelAABB.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/release/LowLevelCloth.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/release/LowLevelDynamics.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/release/LowLevelParticles.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/release/PhysX3Extensions.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/release/PhysX3Vehicle.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/release/SceneQuery.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/release/SimulationController.lib")

        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/release/PhysX3CharacterKinematic_x64.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/release/PhysX3Common_x64.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/release/PhysX3Cooking_x64.lib")
        #pragma comment(lib,"PhysX/PhysX_3.4/lib/vc15win64/release/PhysX3_x64.lib")

    #endif

#endif
