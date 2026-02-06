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
                           static_cast<uint64_t>(MASK_SHOT_PHYSICS),
                           COLLISION_GROUP_DEFAULT, true)
        {
        }

        CTraceFilterEx() : CTraceFilter(static_cast<uint64_t>(MASK_SHOT_PHYSICS), COLLISION_GROUP_DEFAULT, true)
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
        bool TraceShape(
            const Vector* origin,
            const QAngle* viewangles,
            CBaseEntity* ignorePlayer,
            const TraceOptions* opts,
            TraceResult* outResult) override
        {
            auto result = RayTrace::TraceShape(*origin, *viewangles, ignorePlayer, opts);
            if (!result.has_value())
                return false;

            *outResult = result.value();
            return true;
        }

        bool TraceEndShape(
            const Vector* origin,
            const Vector* endOrigin,
            CBaseEntity* ignorePlayer,
            const TraceOptions* opts,
            TraceResult* outResult) override
        {
            auto result = RayTrace::TraceEndShape(*origin, *endOrigin, ignorePlayer, opts);
            if (!result.has_value())
                return false;

            *outResult = result.value();
            return true;
        }

        bool TraceShapeEx(
            const Vector* vecStart,
            const Vector* vecEnd,
            CTraceFilter* filterInc,
            Ray_t* rayInc,
            TraceResult* outResult) override
        {
            auto result = RayTrace::TraceShapeEx(*vecStart, *vecEnd, *filterInc, *rayInc);
            if (!result.has_value())
                return false;

            *outResult = result.value();
            return true;
        }
    };

    extern CRayTrace g_CRayTrace;
}
