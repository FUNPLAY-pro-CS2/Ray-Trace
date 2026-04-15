//
// Created by Michal Přikryl on 30.08.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <optional>
#include "vector.h"
#include "schema/CBaseEntity.h"
#include "craytraceinterface.h"

namespace RayTracePlugin::RayTrace
{
    class CTraceFilterEx : public CTraceFilter
    {
    public:
        explicit CTraceFilterEx(CBaseEntity* entityToIgnore)
            : CTraceFilter(static_cast<CEntityInstance*>(entityToIgnore),
                           entityToIgnore ? entityToIgnore->m_hOwnerEntity.Get() : nullptr,
                           entityToIgnore ? entityToIgnore->m_pCollision()->m_collisionAttribute().m_nHierarchyId() : static_cast<uint16>(0xFFFFFFFF),
                           static_cast<uint64_t>(MASK_SHOT_PHYSICS),
                           COLLISION_GROUP_DEFAULT, true)
        {
        }

        CTraceFilterEx() : CTraceFilter(static_cast<uint64_t>(MASK_SHOT_PHYSICS), COLLISION_GROUP_DEFAULT, true)
        {
        }
    };

    class CRayTrace : public CRayTraceInterface {
    public:
        bool Initialize();
    public:
        bool TraceShape(const Vector& vecStart, const QAngle& angAngles, CEntityInstance* pIgnoreEntity, TraceOptions* pTraceOptions, TraceResult* pTraceResult) override;
        bool TraceEndShape(const Vector& vecStart, const Vector& vecEnd, CEntityInstance* pIgnoreEntity, TraceOptions* pTraceOptions, TraceResult* pTraceResult) override;
        bool TraceHullShape(const Vector& vecStart, const Vector& vecEnd, const Vector& vecMins, const Vector& vecMaxs, CEntityInstance* pIgnoreEntity, TraceOptions* pTraceOptions, TraceResult* pTraceResult) override;
        bool TraceShapeEx(const Vector& vecStart, const Vector& vecEnd, CTraceFilter* pTraceFilter, Ray_t* pRay, TraceResult* pTraceResult) override;
    protected:
        DynLibUtils::CMemory m_pCNavPhysicsInterface_TraceShape;
    protected:
        void** m_pCNavPhysicsInterfaceVTable;
    };

    extern CRayTrace g_CRayTrace;
}
