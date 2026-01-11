//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include "CBaseModelEntity.h"

namespace TemplatePlugin {
    class CParticleSystem : public CBaseModelEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CParticleSystem);

        SCHEMA_FIELD(bool, m_bActive)
        SCHEMA_FIELD(bool, m_bStartActive)
        SCHEMA_FIELD(bool, m_bFrozen)
        SCHEMA_FIELD(float32, m_flFreezeTransitionDuration)
        SCHEMA_FIELD(int32, m_nStopType)
        SCHEMA_FIELD(bool, m_bAnimateDuringGameplayPause)
        SCHEMA_FIELD(CUtlSymbolLarge, m_iszEffectName)
        SCHEMA_FIELD(int, m_nTintCP)
        SCHEMA_FIELD(GameTime_t, m_flStartTime)
        SCHEMA_FIELD_POINTER(Color, m_clrTint)
        SCHEMA_FIELD_POINTER(CHandle<CBaseEntity>, m_hControlPointEnts)
    };

    class CEnvParticleGlow : public CParticleSystem
    {
    public:
        DECLARE_SCHEMA_CLASS(CEnvParticleGlow);

        SCHEMA_FIELD(float, m_flAlphaScale)
        SCHEMA_FIELD(float, m_flRadiusScale)
        SCHEMA_FIELD(float, m_flSelfIllumScale)
        SCHEMA_FIELD_POINTER(Color, m_ColorTint)
    };
}