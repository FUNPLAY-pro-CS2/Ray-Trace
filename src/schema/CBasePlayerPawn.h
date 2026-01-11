//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBaseEntity.h"
#include "CBaseModelEntity.h"
#include "services.h"

namespace TemplatePlugin {
    enum class Hull_t : uint32
    {
        HULL_HUMAN = 0,
        HULL_SMALL_CENTERED = 1,
        HULL_WIDE_HUMAN = 2,
        HULL_TINY = 3,
        HULL_MEDIUM = 4,
        HULL_TINY_CENTERED = 5,
        HULL_LARGE = 6,
        HULL_LARGE_CENTERED = 7,
        HULL_MEDIUM_TALL = 8,
        HULL_SMALL = 9,
        NUM_HULLS = 10,
        HULL_NONE = 11,
    };

    class CMovementStatsProperty
    {
    public:
        DECLARE_SCHEMA_CLASS(CMovementStatsProperty);

        SCHEMA_FIELD(uint32, m_nUseCounter);
        SCHEMA_FIELD(void*, m_emaMovementDirection);
    };

    class CBaseCombatCharacter : public CBaseModelEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CBaseCombatCharacter);

        SCHEMA_FIELD(bool, m_bForceServerRagdoll);
        SCHEMA_FIELD_POINTER(CUtlVector<CHandle<CEconWearable>>, m_hMyWearables)
        SCHEMA_FIELD(float, m_impactEnergyScale);
        SCHEMA_FIELD(bool, m_bApplyStressDamage);
        SCHEMA_FIELD(bool, m_bDeathEventsDispatched);
        SCHEMA_FIELD(CUtlString, m_strRelationships);
        SCHEMA_FIELD(Hull_t, m_eHull);
        SCHEMA_FIELD(uint32, m_nNavHullIdx);
        SCHEMA_FIELD(CMovementStatsProperty, m_movementStats);
    };

    class CBasePlayerPawn : public CBaseCombatCharacter
    {
    public:
        DECLARE_SCHEMA_CLASS(CBasePlayerPawn);

        SCHEMA_FIELD(CCSPlayer_MovementServices*, m_pMovementServices)
        SCHEMA_FIELD(CCSPlayer_WeaponServices*, m_pWeaponServices)
        SCHEMA_FIELD(CCSPlayer_ItemServices*, m_pItemServices)
        SCHEMA_FIELD(CPlayer_ObserverServices*, m_pObserverServices)
        SCHEMA_FIELD(CPlayer_CameraServices*, m_pCameraServices)
        SCHEMA_FIELD(CHandle<CBasePlayerController>, m_hController)
        SCHEMA_FIELD(QAngle, v_angle)
        SCHEMA_FIELD(QAngle, v_anglePrevious)
        SCHEMA_FIELD(uint32, m_iHideHUD)
        SCHEMA_FIELD(bool, m_fInitHUD)

        void CommitSuicide(bool bExplode, bool bForce)
        {
            static int offset = shared::g_pGameConfig->GetOffset("CBasePlayerPawn_CommitSuicide");
            CALL_VIRTUAL(void, offset, this, bExplode, bForce);
        }

        CBasePlayerController *GetController() { return m_hController.Get(); }
    };
}
