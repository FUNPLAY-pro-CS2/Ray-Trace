//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBaseModelEntity.h"
#include "CBasePlayerPawn.h"
#include "globaltypes.h"

namespace TemplatePlugin {
    class CBaseGrenade : public CBaseModelEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CBaseGrenade);
        SCHEMA_FIELD(CHandle<CCSPlayerPawn>, m_hThrower);
        SCHEMA_FIELD(float, m_flDamage);
        SCHEMA_FIELD(float, m_DmgRadius);
    };

    class CBaseCSGrenadeProjectile : public CBaseGrenade
    {
    public:
        DECLARE_SCHEMA_CLASS(CBaseCSGrenadeProjectile);
    };

    class CSmokeGrenadeProjectile : public CBaseCSGrenadeProjectile
    {
    public:
        DECLARE_SCHEMA_CLASS(CSmokeGrenadeProjectile);
        SCHEMA_FIELD(Vector, m_vSmokeColor);
        SCHEMA_FIELD(int, m_nSmokeEffectTickBegin);
    };
}