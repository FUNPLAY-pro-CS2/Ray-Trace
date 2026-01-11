//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBaseEntity.h"
#include "globaltypes.h"

namespace
RayTracePlugin
{
    class CBaseModelEntity : public CBaseEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CBaseModelEntity);

        SCHEMA_FIELD(CCollisionProperty, m_Collision)

        SCHEMA_FIELD(CGlowProperty, m_Glow)

        SCHEMA_FIELD(Color, m_clrRender)

        SCHEMA_FIELD(RenderMode_t, m_nRenderMode)

        SCHEMA_FIELD(RenderFx_t, m_nRenderFX)

        SCHEMA_FIELD(float, m_flDissolveStartTime)

        SCHEMA_FIELD(Vector, m_vecViewOffset)

        SCHEMA_FIELD(float32, m_flShadowStrength)

        void SetModel(const char* model)
        {
            using SetModel_t = void (*)(CBaseModelEntity*, const char*);
            static SetModel_t s_SetModel = nullptr;

            if (!s_SetModel)
            {
                SetModel_t addr =
                    DynLibUtils::CModule(shared::g_pServer)
                    .FindPattern(shared::g_pGameConfig->GetSignature("CBaseModelEntity_SetModel"))
                    .RCast<SetModel_t>();

                if (!addr) return;
                s_SetModel = addr;
            }

            s_SetModel(this, model);
        }

        void SetBodyGroup(std::string name, int value)
        {
            char bodygroupStr[64];
            g_SMAPI->Format(bodygroupStr, sizeof(bodygroupStr), "%s,%i", name.c_str(), value);
            this->AcceptInput(
                "SetBodyGroup",
                this,
                this,
                bodygroupStr
            );
        }

        CUtlSymbolLarge GetModelName()
        {
            if (m_CBodyComponent == nullptr) return CUtlSymbolLarge();
            if (m_CBodyComponent->m_pSceneNode == nullptr) return CUtlSymbolLarge();
            if (((CSkeletonInstance*)m_CBodyComponent->m_pSceneNode.Get()) == nullptr) return CUtlSymbolLarge();
            return ((CSkeletonInstance*)m_CBodyComponent->m_pSceneNode.Get())->m_modelState().m_ModelName.Get();
        }

        Vector GetEyePosition()
        {
            const auto x = m_vecViewOffset();
            return x + GetAbsOrigin();
        }
    };

    class CBeam : public CBaseModelEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CBeam);
        SCHEMA_FIELD(Vector, m_vecEndPos);

        SCHEMA_FIELD(float, m_fWidth);

        SCHEMA_FIELD(float, m_fEndWidth);
    };
}
