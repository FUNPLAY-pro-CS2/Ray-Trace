//
// Created by Michal Přikryl on 30.08.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <optional>
#include "vector.h"
#include "schema/CBaseEntity.h"
#include "CRayTraceInterface.h"

namespace RayTracePlugin::RayTrace
{
    class CTraceFilterEx : public CTraceFilter
    {
    public:
        explicit CTraceFilterEx(CBaseEntity* entityToIgnore)
            : CTraceFilter(static_cast<CEntityInstance*>(entityToIgnore),
                           entityToIgnore ? entityToIgnore->m_hOwnerEntity.Get() : nullptr,
                           entityToIgnore ? entityToIgnore->m_pCollision()->m_collisionAttribute().m_nHierarchyId() : static_cast<uint16>(0xFFFFFFFF),
                           0x2c3011,
                           COLLISION_GROUP_DEFAULT, true)
        {
        }

        CTraceFilterEx() : CTraceFilter(0x2c3011, COLLISION_GROUP_DEFAULT, true)
        {
        }
    };

    bool Initialize();

    std::optional<TraceResult> TraceShape(
        const Vector& origin,
        const QAngle& viewangles,
        CBaseEntity* ignorePlayer = nullptr,
        const TraceOptions* opts = nullptr);

    std::optional<TraceResult> TraceEndShape(
        const Vector& origin,
        const Vector& endOrigin,
        CBaseEntity* ignorePlayer = nullptr,
        const TraceOptions* opts = nullptr);

    std::optional<TraceResult> TraceShapeEx(
        const Vector& vecStart,
        const Vector& vecEnd,
        CTraceFilter& filterInc,
        Ray_t rayInc);

    class CRayTrace : public CRayTraceInterface
    {
    public:
        std::optional<TraceResult> TraceShape(
            const Vector& origin,
            const QAngle& viewangles,
            CBaseEntity* ignorePlayer,
            const TraceOptions* opts) override
        {
            return RayTrace::TraceShape(origin, viewangles, ignorePlayer, opts);
        }

        std::optional<TraceResult> TraceEndShape(
            const Vector& origin,
            const Vector& endOrigin,
            CBaseEntity* ignorePlayer,
            const TraceOptions* opts) override
        {
            return RayTrace::TraceEndShape(origin, endOrigin, ignorePlayer, opts);
        }

        std::optional<TraceResult> TraceShapeEx(
            const Vector& vecStart,
            const Vector& vecEnd,
            CTraceFilter& filterInc,
            Ray_t rayInc) override
        {
            return RayTrace::TraceShapeEx(vecStart, vecEnd, filterInc, rayInc);
        }
    };

    inline CRayTrace g_CRayTrace;
}
