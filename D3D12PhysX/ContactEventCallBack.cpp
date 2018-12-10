#include "stdafx.h"
#include "ContactEventCallBack.h"

void ContactEventCallBack::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) 
{}

void ContactEventCallBack::onWake(PxActor** actors, PxU32 count)
{}

void ContactEventCallBack::onSleep(PxActor** actors, PxU32 count) 
{}

void ContactEventCallBack::onAdvance(const PxRigidBody*const*, const PxTransform*, const PxU32)
{}

void ContactEventCallBack::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
    printf("Trigger �� �߻���\n");
}

void ContactEventCallBack::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
    ///////////////�浹 �̺�Ʈ ����///////////////////
    PxRigidActor * A = pairHeader.actors[0];
    PxRigidActor * B = pairHeader.actors[1];

    ///////////////����浹..��¾���//////////////////
    if (strcmp(A->getName(), "���") == 0 || strcmp(B->getName(), "���") == 0) return;

    printf("ù��° ���� :%s, �ι�° ���� : %s\n", A->getName(), B->getName());

    for (unsigned int i = 0; i < nbPairs; i++)
    {
        if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
        {
            printf("�浹�� ���۵�\n");
        }

        if (pairs[i].events &  PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
        {
            printf("�浹��..��� �ǰ� ����\n");
        }

        if (pairs[i].events &  PxPairFlag::eNOTIFY_TOUCH_LOST)
        {
            printf("�浹�� ����\n");
        }
      
        //////////////////////�浹 ����Ʈ//////////////////////////////
        const PxU32 bufferSize    = 64;
        PxContactPairPoint contacts[bufferSize];

        PxU32 nbContacts = pairs[i].extractContacts(contacts, bufferSize);

        if (nbContacts > 0)
        {
            //printf("�浹 ����Ʈ\n");

            for (PxU32 k = 0; k < nbContacts; k++)
            {
                PxVec3 point   = contacts[k].position;
                PxVec3 impulse = contacts[k].impulse;
                //PxU32 internalFaceIndex0 = contacts[j].internalFaceIndex0;
                //PxU32 internalFaceIndex1 = contacts[j].internalFaceIndex1;
                printf("  �浹 ����Ʈ [%d] (%f %f %f)\n" , k, point.x, point.y, point.z);
                //printf("  ��·�   [%d] (%f %f %f)\n",  k, impulse.x,  impulse.y, impulse.z);
            }
        }
    }
}

PxFilterFlags contactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0,
                                        PxFilterObjectAttributes attributes1, PxFilterData filterData1,
                                        PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
    // all initial and persisting reports for everything, with per-point data
    pairFlags  = PxPairFlag::eSOLVE_CONTACT;
    pairFlags |= PxPairFlag::eDETECT_DISCRETE_CONTACT;
    pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
    pairFlags |= PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
    pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
    pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;

    return PxFilterFlag::eDEFAULT;
}