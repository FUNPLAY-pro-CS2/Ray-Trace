//
// Created by Michal Přikryl on 30.08.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "raytrace.h"
#include <shared.h>
#include "schema/CBaseModelEntity.h"
#include "vectorextends.h"
#include "dynlibutils/memaddr.h"
#include "dynlibutils/module.h"
#include "colors.h"
#include "log.h"

namespace RayTracePlugin::RayTrace
{
    CRayTrace g_CRayTrace;

    bool CRayTrace::Initialize()
    {
        m_pCNavPhysicsInterfaceVTable = DynLibUtils::CModule(shared::g_pServer).GetVirtualTableByName("CNavPhysicsInterface").RCast<void**>();
        if (!m_pCNavPhysicsInterfaceVTable)
        {
            FP_WARN("Tried getting virtual function from a null vtable.");
            return false;
        }

        m_pCNavPhysicsInterface_TraceShape = m_pCNavPhysicsInterfaceVTable[shared::g_pGameConfig->GetOffset("CNavPhysicsInterface_TraceShape")];
        return true;
    }

    static void DrawBeam(const Vector& start, const Vector& end, const Color& color)
    {
        CBeam* beam = UTIL_CreateEntityByName<CBeam>("env_beam");
        if (!beam) return;

        beam->m_clrRender().SetColor(color.r(), color.g(), color.b(), color.a());
        beam->m_fWidth() = 1.5f;
        beam->m_nRenderMode() = kRenderNormal;
        beam->m_nRenderFX() = kRenderFxNone;

        beam->Teleport(&start, &VectorExtends::RotationZero, &VectorExtends::VectorZero);
        beam->m_vecEndPos() = end;
        beam->DispatchSpawn();
    }

    TraceResult CRayTrace::TraceShape(const Vector& vecStart, const QAngle& angAngles, CEntityInstance* pIgnoreEntity, TraceOptions* pTraceOptions)
    {
        CTraceFilterEx filter = pIgnoreEntity ? CTraceFilterEx(static_cast<CBaseEntity*>(pIgnoreEntity)) : CTraceFilterEx();

        filter.m_nInteractsAs = 0;
        filter.m_nInteractsWith = static_cast<uint64_t>(MASK_SHOT_PHYSICS);
        filter.m_nInteractsExclude = 0;

        if (pTraceOptions)
        {
            if (pTraceOptions->InteractsAs != 0)
                filter.m_nInteractsAs = pTraceOptions->InteractsAs;

            if (pTraceOptions->InteractsWith != static_cast<uint64_t>(MASK_SHOT_PHYSICS))
                filter.m_nInteractsWith = pTraceOptions->InteractsWith;

            if (pTraceOptions->InteractsExclude != 0)
                filter.m_nInteractsExclude = pTraceOptions->InteractsExclude;
        }

        Vector forward;
        AngleVectors(angAngles, &forward);
        Vector vecEnd{
            vecStart.x + forward.x * 8192.f,
            vecStart.y + forward.y * 8192.f,
            vecStart.z + forward.z * 8192.f
        };

        Ray_t ray;
        auto res = TraceShapeEx(vecStart, vecEnd, &filter, &ray);

        if (pTraceOptions && pTraceOptions->DrawBeam)
        {
            Color col = res.DidHit() ? colors::Red().ToValveColor() : colors::Green().ToValveColor();
            DrawBeam(vecStart, res.DidHit() ? res.HitPoint() : vecEnd, col);
        }

        return res;
    }

    TraceResult CRayTrace::TraceEndShape(const Vector& vecStart, const Vector& vecEnd, CEntityInstance* pIgnoreEntity, TraceOptions* pTraceOptions)
    {
        CTraceFilterEx filter = pIgnoreEntity ? CTraceFilterEx(static_cast<CBaseEntity*>(pIgnoreEntity)) : CTraceFilterEx();

        filter.m_nInteractsAs = 0;
        filter.m_nInteractsWith = static_cast<uint64_t>(MASK_SHOT_PHYSICS);
        filter.m_nInteractsExclude = 0;

        if (pTraceOptions)
        {
            if (pTraceOptions->InteractsAs != 0)
                filter.m_nInteractsAs = pTraceOptions->InteractsAs;

            if (pTraceOptions->InteractsWith != static_cast<uint64_t>(MASK_SHOT_PHYSICS))
                filter.m_nInteractsWith = pTraceOptions->InteractsWith;

            if (pTraceOptions->InteractsExclude != 0)
                filter.m_nInteractsExclude = pTraceOptions->InteractsExclude;
        }

        Ray_t ray;
        auto res = TraceShapeEx(vecStart, vecEnd, &filter, &ray);

        if (pTraceOptions && pTraceOptions->DrawBeam)
        {
            Color col = res.DidHit() ? colors::Red().ToValveColor() : colors::Green().ToValveColor();
            DrawBeam(vecStart, res.DidHit() ? res.HitPoint() : vecEnd, col);
        }

        return res;
    }

    TraceResult CRayTrace::TraceHullShape(const Vector& vecStart, const Vector& vecEnd, const Vector& vecMins,
                                          const Vector& vecMaxs, CEntityInstance* pIgnoreEntity, TraceOptions* pTraceOptions)
    {
        CTraceFilterEx filter = pIgnoreEntity ? CTraceFilterEx(static_cast<CBaseEntity*>(pIgnoreEntity)) : CTraceFilterEx();

        filter.m_nInteractsAs = 0;
        filter.m_nInteractsWith = static_cast<uint64_t>(MASK_SHOT_PHYSICS);
        filter.m_nInteractsExclude = 0;

        if (pTraceOptions)
        {
            if (pTraceOptions->InteractsAs != 0)
                filter.m_nInteractsAs = pTraceOptions->InteractsAs;

            if (pTraceOptions->InteractsWith != static_cast<uint64_t>(MASK_SHOT_PHYSICS))
                filter.m_nInteractsWith = pTraceOptions->InteractsWith;

            if (pTraceOptions->InteractsExclude != 0)
                filter.m_nInteractsExclude = pTraceOptions->InteractsExclude;
        }

        Ray_t ray;
        ray.Init(vecMins, vecMaxs);

        auto res = TraceShapeEx(vecStart, vecEnd, &filter, &ray);

        if (pTraceOptions && pTraceOptions->DrawBeam)
        {
            Color col = res.DidHit() ? colors::Red().ToValveColor() : colors::Green().ToValveColor();
            DrawBeam(vecStart, res.DidHit() ? res.HitPoint() : vecEnd, col);
        }

        return res;
    }

    TraceResult CRayTrace::TraceShapeEx(const Vector& vecStart, const Vector& vecEnd, CTraceFilter* pTraceFilter, Ray_t* pRay)
    {
        if (!m_pCNavPhysicsInterface_TraceShape)
        {
            FP_ERROR("CNavPhysicsInterface::TraceShape is not bound!");
            return TraceResult();
        }

        Vector vecStartCopy = vecStart;
        Vector vecEndCopy = vecEnd;
        CGameTrace trace;

        bool bResult = m_pCNavPhysicsInterface_TraceShape.RCast<
            bool (*)(void*, Ray_t&, Vector&, Vector&, CTraceFilter*, CGameTrace*)>()(
            nullptr, *pRay, vecStartCopy, vecEndCopy, pTraceFilter, &trace);

        return TraceResult(&trace, bResult);
    }
}
