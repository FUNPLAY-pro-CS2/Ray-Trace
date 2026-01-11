//
// Created by Michal Přikryl on 01.11.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBaseEntity.h"
#include "globaltypes.h"
#include "CBreakableProp.h"

namespace
TemplatePlugin
{
    enum DynamicContinuousContactBehavior_t : byte
    {
        DYNAMIC_CONTINUOUS_ALLOW_IF_REQUESTED_BY_OTHER_BODY = 0x0,
        DYNAMIC_CONTINUOUS_ALWAYS = 0x1,
        DYNAMIC_CONTINUOUS_NEVER = 0x2,
    };

    enum CPhysicsPropCrateType_t : uint
    {
        CRATE_SPECIFIC_ITEM = 0x0,
        CRATE_TYPE_COUNT = 0x1,
    };

    class CPhysicsProp : public CBreakableProp
    {
    public:
        DECLARE_SCHEMA_CLASS(CPhysicsProp);

        SCHEMA_FIELD(CEntityIOOutput, m_MotionEnabled)
        SCHEMA_FIELD(CEntityIOOutput, m_OnAwakened)
        SCHEMA_FIELD(CEntityIOOutput, m_OnAwake)
        SCHEMA_FIELD(CEntityIOOutput, m_OnAsleep)
        SCHEMA_FIELD(CEntityIOOutput, m_OnPlayerUse)
        SCHEMA_FIELD(CEntityIOOutput, m_OnOutOfWorld)
        SCHEMA_FIELD(CEntityIOOutput, m_OnPlayerPickup)
        SCHEMA_FIELD(bool, m_bForceNavIgnore)
        SCHEMA_FIELD(bool, m_bNoNavmeshBlocker)
        SCHEMA_FIELD(bool, m_bForceNpcExclude)
        SCHEMA_FIELD(float, m_massScale)
        SCHEMA_FIELD(float, m_buoyancyScale)
        SCHEMA_FIELD(int32, m_damageType)
        SCHEMA_FIELD(int32, m_damageToEnableMotion)
        SCHEMA_FIELD(float, m_flForceToEnableMotion)
        SCHEMA_FIELD(bool, m_bThrownByPlayer)
        SCHEMA_FIELD(bool, m_bDroppedByPlayer)
        SCHEMA_FIELD(bool, m_bTouchedByPlayer)
        SCHEMA_FIELD(bool, m_bFirstCollisionAfterLaunch)
        SCHEMA_FIELD(bool, m_bHasBeenAwakened)
        SCHEMA_FIELD(bool, m_bIsOverrideProp)
        SCHEMA_FIELD(float, m_flLastBurn)
        SCHEMA_FIELD(DynamicContinuousContactBehavior_t, m_nDynamicContinuousContactBehavior)
        SCHEMA_FIELD(float, m_fNextCheckDisableMotionContactsTime)
        SCHEMA_FIELD(int32, m_iInitialGlowState)
        SCHEMA_FIELD(int32, m_nGlowRange)
        SCHEMA_FIELD(int32, m_nGlowRangeMin)
        SCHEMA_FIELD(Color, m_glowColor)
        SCHEMA_FIELD(bool, m_bShouldAutoConvertBackFromDebris)
        SCHEMA_FIELD(bool, m_bMuteImpactEffects)
        SCHEMA_FIELD(bool, m_bAcceptDamageFromHeldObjects)
        SCHEMA_FIELD(bool, m_bEnableUseOutput)
        SCHEMA_FIELD(CPhysicsPropCrateType_t, m_CrateType)
        SCHEMA_FIELD_POINTER(char, m_strItemClass)
        SCHEMA_FIELD_POINTER(int32, m_nItemCount)
        SCHEMA_FIELD(bool, m_bRemovableForAmmoBalancing)
        SCHEMA_FIELD(bool, m_bAwake)
        SCHEMA_FIELD(bool, m_bAttachedToReferenceFrame)
    };
}
