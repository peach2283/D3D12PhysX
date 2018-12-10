#pragma once

#include <PxPhysicsAPI.h>

using namespace physx;

class ContactEventCallBack : public  PxSimulationEventCallback
{
public:

    void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count);
    void onWake(PxActor** actors, PxU32 count);
    void onSleep(PxActor** actors, PxU32 count);
    void onTrigger(PxTriggerPair* pairs, PxU32 count);
    void onAdvance(const PxRigidBody*const*, const PxTransform*, const PxU32);
    void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);

};

PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
                                        PxFilterObjectAttributes attributes1, PxFilterData filterData1,
                                        PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize);
