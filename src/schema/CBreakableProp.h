//
// Created by Michal Přikryl on 01.11.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBaseEntity.h"
#include "globaltypes.h"
#include "CBaseProp.h"

namespace
TemplatePlugin
{
    class CPropDataComponent  : public CEntityComponent
    {
    public:
        DECLARE_SCHEMA_CLASS(CPropDataComponent)

        SCHEMA_FIELD(float, m_flDmgModBullet)
        SCHEMA_FIELD(float, m_flDmgModClub)
        SCHEMA_FIELD(float, m_flDmgModExplosive)
        SCHEMA_FIELD(float, m_flDmgModFire)
        SCHEMA_FIELD_POINTER(char, m_iszPhysicsDamageTableName)
        SCHEMA_FIELD_POINTER(char, m_iszBasePropData)
        SCHEMA_FIELD(int32, m_nInteractions)
        SCHEMA_FIELD(bool, m_bSpawnMotionDisabled)
        SCHEMA_FIELD(int32, m_nDisableTakePhysicsDamageSpawnFlag)
        SCHEMA_FIELD(int32, m_nMotionDisabledSpawnFlag)
    };

    enum BreakableContentsType_t : uint
    {
        BC_DEFAULT = 0x0,
        BC_EMPTY = 0x1,
        BC_PROP_GROUP_OVERRIDE = 0x2,
        BC_PARTICLE_SYSTEM_OVERRIDE = 0x3,
    };

    enum PerformanceMode_t : uint
    {
        PM_NORMAL = 0x0,
        PM_NO_GIBS = 0x1,
    };

    class CBreakableProp : public CBaseProp
    {
    public:
        DECLARE_SCHEMA_CLASS(CBreakableProp);

        SCHEMA_FIELD(CPropDataComponent, m_CPropDataComponent)
        SCHEMA_FIELD(CEntityIOOutput, m_OnStartDeath)
        SCHEMA_FIELD(CEntityIOOutput, m_OnBreak)
        SCHEMA_FIELD(CEntityIOOutput, m_OnTakeDamage)
        SCHEMA_FIELD(float, m_impactEnergyScale)
        SCHEMA_FIELD(int32, m_iMinHealthDmg)
        SCHEMA_FIELD(QAngle, m_preferredCarryAngles)
        SCHEMA_FIELD(float, m_flPressureDelay)
        SCHEMA_FIELD(float, m_flDefBurstScale)
        SCHEMA_FIELD(Vector, m_vDefBurstOffset)
        SCHEMA_FIELD(CHandle<CBaseEntity>, m_hBreaker)
        SCHEMA_FIELD(PerformanceMode_t, m_PerformanceMode)
        SCHEMA_FIELD(float, m_flPreventDamageBeforeTime)
        SCHEMA_FIELD(BreakableContentsType_t, m_BreakableContentsType)
        SCHEMA_FIELD_POINTER(char, m_strBreakableContentsPropGroupOverride)
        SCHEMA_FIELD_POINTER(char, m_strBreakableContentsParticleOverride)
        SCHEMA_FIELD(bool, m_bHasBreakPiecesOrCommands)
        SCHEMA_FIELD(float, m_explodeDamage)
        SCHEMA_FIELD(float, m_explodeRadius)
        SCHEMA_FIELD(float, m_explosionDelay)
        SCHEMA_FIELD_POINTER(char, m_explosionBuildupSound)
        SCHEMA_FIELD_POINTER(char, m_explosionCustomEffect)
        SCHEMA_FIELD_POINTER(char, m_explosionCustomSound)
        SCHEMA_FIELD_POINTER(char, m_explosionModifier)
        SCHEMA_FIELD(CHandle<CBasePlayerPawn>, m_hPhysicsAttacker)
        SCHEMA_FIELD(float, m_flLastPhysicsInfluenceTime)
        SCHEMA_FIELD(float, m_flDefaultFadeScale)
        SCHEMA_FIELD(CHandle<CBaseEntity>, m_hLastAttacker)
        SCHEMA_FIELD_POINTER(char, m_iszPuntSound)
        SCHEMA_FIELD(bool, m_bUsePuntSound)
        SCHEMA_FIELD(bool, m_bOriginalBlockLOS)
    };
}
