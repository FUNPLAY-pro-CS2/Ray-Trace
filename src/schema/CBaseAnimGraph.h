//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBaseEntity.h"
#include "globaltypes.h"
#include "CBaseModelEntity.h"

namespace
TemplatePlugin
{
	class IChoreoServices
    {
	    DECLARE_SCHEMA_CLASS(IChoreoServices)
    };

    class PhysicsRagdollPose_t
    {
        DECLARE_SCHEMA_CLASS(PhysicsRagdollPose_t)

        SCHEMA_FIELD_POINTER(CUtlVector<CTransform>, m_Transforms)
        SCHEMA_FIELD(CHandle<CBaseEntity>, m_hOwner)
        SCHEMA_FIELD(bool, m_bSetFromDebugHistory)
    };

    class CBaseAnimGraph : public CBaseModelEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CBaseAnimGraph)

        SCHEMA_FIELD(bool, m_bInitiallyPopulateInterpHistory)
        SCHEMA_FIELD(IChoreoServices *, m_pChoreoServices)
        SCHEMA_FIELD(bool, m_bAnimGraphUpdateEnabled)
        SCHEMA_FIELD(float, m_flMaxSlopeDistance)
        SCHEMA_FIELD(Vector, m_vLastSlopeCheckPos)
        SCHEMA_FIELD(bool, m_bAnimationUpdateScheduled)
        SCHEMA_FIELD(Vector, m_vecForce)
        SCHEMA_FIELD(int32, m_nForceBone)
        SCHEMA_FIELD(PhysicsRagdollPose_t, m_RagdollPose)
        SCHEMA_FIELD(bool, m_bRagdollEnabled)
        SCHEMA_FIELD(bool, m_bRagdollClientSide)
        SCHEMA_FIELD(CTransform, m_xParentedRagdollRootInEntitySpace)
    };
}
