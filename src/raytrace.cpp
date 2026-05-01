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

    void CRayTrace::DrawBeam(const Vector& vecStart, const Vector& vecEnd, const Color& colRender)
    {
        CBeam* beam = UTIL_CreateEntityByName<CBeam>("env_beam");
        if (!beam) return;

        beam->m_clrRender() = colRender;
        beam->m_fWidth() = 1.5f;
        beam->m_nRenderMode() = kRenderNormal;
        beam->m_nRenderFX() = kRenderFxNone;

        beam->Teleport(&vecStart, &VectorExtends::RotationZero, &VectorExtends::VectorZero);
        beam->m_vecEndPos() = vecEnd;
        beam->DispatchSpawn();
    }

    bool CRayTrace::TraceShape(const Vector* pVecStart, const QAngle* pAngAngles, CEntityInstance* pIgnoreEntity, TraceOptions* pTraceOptions, TraceResult* pTraceResult)
    {
        auto result = TraceShapeInternal(*pVecStart, *pAngAngles, pIgnoreEntity, pTraceOptions);
        if (!result.has_value())
            return false;

        *pTraceResult = result.value();
        return true;
    }

    bool CRayTrace::TraceEndShape(const Vector* pVecStart, const Vector* pVecEnd, CEntityInstance* pIgnoreEntity, TraceOptions* pTraceOptions, TraceResult* pTraceResult)
    {
        auto result = TraceEndShapeInternal(*pVecStart, *pVecEnd, pIgnoreEntity, pTraceOptions);
        if (!result.has_value())
            return false;

        *pTraceResult = result.value();
        return true;
    }

    bool CRayTrace::TraceHullShape(const Vector* pVecStart, const Vector* pVecEnd, const Vector* pVecMins,
                                          const Vector* pVecMaxs, CEntityInstance* pIgnoreEntity, TraceOptions* pTraceOptions, TraceResult* pTraceResult)
    {
        auto result = TraceHullShapeInternal(*pVecStart, *pVecEnd, *pVecMins, *pVecMaxs, pIgnoreEntity, pTraceOptions);
        if (!result.has_value())
            return false;

        *pTraceResult = result.value();
        return true;
    }

    bool CRayTrace::TraceShapeEx(const Vector* pVecStart, const Vector* pVecEnd, CTraceFilter* pTraceFilter, Ray_t* pRay, TraceResult* pTraceResult)
    {
        auto result = TraceShapeExInternal(*pVecStart, *pVecEnd, *pTraceFilter, *pRay);
        if (!result.has_value())
            return false;

        *pTraceResult = result.value();
        return true;
    }

    std::optional<TraceResult> CRayTrace::TraceShapeInternal(const Vector& vecStart, const QAngle& angAngles, CEntityInstance* pIgnoreEntity, const TraceOptions* pTraceOptions)
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

        Vector vecForward;
        AngleVectors(angAngles, &vecForward);
        Vector vecEnd{
            vecStart.x + vecForward.x * 8192.f,
            vecStart.y + vecForward.y * 8192.f,
            vecStart.z + vecForward.z * 8192.f
        };

        Ray_t ray;
        auto res = TraceShapeExInternal(vecStart, vecEnd, filter, ray);

        if (pTraceOptions && pTraceOptions->DrawBeam)
        {
            Color col = res.has_value() ? colors::Red().ToValveColor() : colors::Green().ToValveColor();
            DrawBeam(vecStart, res ? Vector(res->EndPosX, res->EndPosY, res->EndPosZ) : vecEnd, col);
        }

        return res;
    }

    std::optional<TraceResult> CRayTrace::TraceEndShapeInternal(const Vector& vecStart, const Vector& vecEnd, CEntityInstance* pIgnoreEntity, const TraceOptions* pTraceOptions)
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
        auto res = TraceShapeExInternal(vecStart, vecEnd, filter, ray);

        if (pTraceOptions && pTraceOptions->DrawBeam)
        {
            Color col = res.has_value() ? colors::Red().ToValveColor() : colors::Green().ToValveColor();
            DrawBeam(vecStart, res ? Vector(res->EndPosX, res->EndPosY, res->EndPosZ) : vecEnd, col);
        }

        return res;
    }

    std::optional<TraceResult> CRayTrace::TraceHullShapeInternal(const Vector& vecStart, const Vector& vecEnd, const Vector& vecMins, const Vector& vecMaxs, CEntityInstance* pIgnoreEntity, const TraceOptions* pTraceOptions)
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
        auto res = TraceShapeExInternal(vecStart, vecEnd, filter, ray);

        if (pTraceOptions && pTraceOptions->DrawBeam)
        {
            Color col = res.has_value() ? colors::Red().ToValveColor() : colors::Green().ToValveColor();
            DrawBeam(vecStart, res ? Vector(res->EndPosX, res->EndPosY, res->EndPosZ) : vecEnd, col);
        }

        return res;
    }

    std::optional<TraceResult> CRayTrace::TraceShapeExInternal(const Vector& vecStart, const Vector& vecEnd, CTraceFilter& traceFilter, Ray_t ray)
    {
        if (!m_pCNavPhysicsInterface_TraceShape)
        {
            FP_ERROR("CNavPhysicsInterface::TraceShape is not bound!");
            return std::nullopt;
        }

        Vector vecStartCopy = vecStart;
        Vector vecEndCopy = vecEnd;
        CGameTrace tr;

        m_pCNavPhysicsInterface_TraceShape.RCast<bool (*)(void*, Ray_t&, Vector&, Vector&, CTraceFilter*, CGameTrace*)>()(nullptr, ray, vecStartCopy, vecEndCopy, &traceFilter, &tr);

        TraceResult r{};
        r.StartPosX = tr.m_vStartPos.x;
        r.StartPosY = tr.m_vStartPos.y;
        r.StartPosZ = tr.m_vStartPos.z;

        r.EndPosX = tr.m_vEndPos.x;
        r.EndPosY = tr.m_vEndPos.y;
        r.EndPosZ = tr.m_vEndPos.z;

        r.HitPointX = tr.m_vHitPoint.x;
        r.HitPointY = tr.m_vHitPoint.y;
        r.HitPointZ = tr.m_vHitPoint.z;

        r.NormalX = tr.m_vHitNormal.x;
        r.NormalY = tr.m_vHitNormal.y;
        r.NormalZ = tr.m_vHitNormal.z;

        r.Fraction = tr.m_flFraction;
        r.HitOffset = tr.m_flHitOffset;

        r.TriangleIndex = tr.m_nTriangle;
        r.HitboxBoneIndex = tr.m_nHitboxBoneIndex;

        r.Contents = tr.m_nContents;

        r.RayType = static_cast<int>(tr.m_eRayType);

        r.AllSolid = tr.m_bStartInSolid;
        r.ExactHitPoint = tr.m_bExactHitPoint;

        r.HitEntity = reinterpret_cast<uintptr_t>(tr.m_pEnt);
        r.Hitbox = reinterpret_cast<uintptr_t>(tr.m_pHitbox);
        r.SurfaceProps = reinterpret_cast<uintptr_t>(tr.m_pSurfaceProperties);
        r.BodyHandle = reinterpret_cast<uintptr_t>(tr.m_hBody);
        r.ShapeHandle = reinterpret_cast<uintptr_t>(tr.m_hShape);
        r.BodyTransform = tr.m_BodyTransform;
        r.ShapeAttributes = tr.m_ShapeAttributes;

        return r;
    }
}
