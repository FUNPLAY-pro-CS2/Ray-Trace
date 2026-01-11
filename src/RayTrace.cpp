//
// Created by Michal Přikryl on 30.08.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "RayTrace.h"
#include <Shared.h>
#include "schema/CBaseModelEntity.h"
#include "vectorextends.h"
#include "dynlibutils/memaddr.h"
#include "dynlibutils/module.h"
#include "colors.h"
#include "log.h"

namespace RayTracePlugin::RayTrace
{
    using TraceShapeFn = bool(*)(void* pThis,
                                 Ray_t& ray,
                                 Vector& start,
                                 Vector& end,
                                 CTraceFilter* filter,
                                 CGameTrace* trace);
    static TraceShapeFn s_TraceShape = nullptr;

    bool Initialize()
    {
        void* pCNavPhysicsInterfaceVTable =
            DynLibUtils::CModule(shared::g_pServer).GetVirtualTableByName("CNavPhysicsInterface");

        if (!pCNavPhysicsInterfaceVTable)
        {
            FP_ERROR("Failed to find CNavPhysicsInterface vtable!");
            return false;
        }

        auto table = static_cast<void**>(pCNavPhysicsInterfaceVTable);
        s_TraceShape = reinterpret_cast<TraceShapeFn>(table[shared::g_pGameConfig->GetOffset("CNavPhysicsInterface_TraceShape")]);

        return true;
    }

    static void DrawBeam(const Vector& start, const Vector& end, const Color& color)
    {
        CBeam* beam = UTIL_CreateEntityByName<CBeam>("env_beam");
        if (!beam) return;

        beam->m_clrRender().SetColor(color.r(), color.g(), color.b(), color.a());
        beam->m_fWidth() = 1.5f;
        beam->m_nRenderMode() = kRenderGlow;
        beam->m_nRenderFX() = kRenderFxNone;

        beam->Teleport(&start, &VectorExtends::RotationZero, &VectorExtends::VectorZero);
        beam->m_vecEndPos() = end;
        beam->DispatchSpawn();
    }

    std::optional<TraceResult> TraceShapeEx(
        const Vector& start,
        const Vector& end,
        CTraceFilter& filterInc,
        Ray_t rayInc)
    {
        if (!s_TraceShape) return std::nullopt;

        CGameTrace tr{};
        Vector startCopy = start;
        Vector endCopy = end;
        s_TraceShape(nullptr, rayInc, startCopy, endCopy,
                     &filterInc, &tr);

        TraceResult r{};
        r.EndPos = tr.m_vEndPos;
        r.HitEntity = tr.m_pEnt;
        r.Fraction = tr.m_flFraction;
        r.AllSolid = tr.m_bStartInSolid;
        r.Normal = tr.m_vHitNormal;

        return r;
    }

    std::optional<TraceResult> TraceShape(
        const Vector& origin,
        const QAngle& viewangles,
        CBaseEntity* ignorePlayer,
        const TraceOptions* opts)
    {
        Vector forward;
        AngleVectors(viewangles, &forward);
        Vector endOrigin{
            origin.x + forward.x * 8192.f,
            origin.y + forward.y * 8192.f,
            origin.z + forward.z * 8192.f
        };

        CTraceFilterEx filter = ignorePlayer ? CTraceFilterEx(ignorePlayer) : CTraceFilterEx();

        if (opts)
        {
            if (opts->InteractsWith) filter.m_nInteractsWith = static_cast<uint64_t>(*opts->InteractsWith);
            if (opts->InteractsExclude) filter.m_nInteractsExclude = static_cast<uint64_t>(*opts->InteractsExclude);
        }

        Ray_t ray;
        auto res = TraceShapeEx(origin, endOrigin, filter, ray);

        if (opts && opts->DrawBeam)
        {
            Color col = res.has_value() ? colors::Red().ToValveColor() : colors::Green().ToValveColor();
            DrawBeam(origin, res ? res->EndPos : endOrigin, col);
        }

        return res;
    }

    std::optional<TraceResult> TraceEndShape(
        const Vector& origin,
        const Vector& endOrigin,
        CBaseEntity* ignorePlayer,
        const TraceOptions* opts)
    {
        CTraceFilterEx filter = ignorePlayer ? CTraceFilterEx(ignorePlayer) : CTraceFilterEx();

        if (opts)
        {
            if (opts->InteractsWith) filter.m_nInteractsWith = static_cast<uint64_t>(*opts->InteractsWith);
            if (opts->InteractsExclude) filter.m_nInteractsExclude = static_cast<uint64_t>(*opts->InteractsExclude);
        }

        Ray_t ray;
        auto res = TraceShapeEx(origin, endOrigin, filter, ray);

        if (opts && opts->DrawBeam)
        {
            Color col = res.has_value() ? colors::Red().ToValveColor() : colors::Green().ToValveColor();
            DrawBeam(origin, res ? res->EndPos : endOrigin, col);
        }

        return res;
    }
}
