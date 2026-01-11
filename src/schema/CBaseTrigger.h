//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include "CBaseModelEntity.h"
#include "schemasystem.h"

#define SF_TRIG_PUSH_ONCE 0x80

namespace TemplatePlugin {
    enum TOGGLE_STATE : uint32_t
    {
        TS_AT_TOP = 0,
        TS_AT_BOTTOM = 1,
        TS_GOING_UP = 2,
        TS_GOING_DOWN = 3,
        DOOR_OPEN = 0,
        DOOR_CLOSED = 1,
        DOOR_OPENING = 2,
        DOOR_CLOSING = 3,
    };

    class CBaseToggle : public CBaseModelEntity
    {
    public:
    DECLARE_SCHEMA_CLASS(CBaseToggle);
        SCHEMA_FIELD(TOGGLE_STATE, m_toggle_state)
        SCHEMA_FIELD(float32, m_flMoveDistance)
        SCHEMA_FIELD(float32, m_flWait)
        SCHEMA_FIELD(float32, m_flLip)
        SCHEMA_FIELD(bool, m_bAlwaysFireBlockedOutputs)
        SCHEMA_FIELD(Vector, m_vecPosition1)
        SCHEMA_FIELD(Vector, m_vecPosition2)
        SCHEMA_FIELD(QAngle, m_vecMoveAng)
        SCHEMA_FIELD(QAngle, m_vecAngle1)
        SCHEMA_FIELD(QAngle, m_vecAngle2)
        SCHEMA_FIELD(float32, m_flHeight)
        SCHEMA_FIELD(CEntityHandle, m_hActivator)
        SCHEMA_FIELD(Vector, m_vecFinalDest)
        SCHEMA_FIELD(QAngle, m_vecFinalAngle)
        SCHEMA_FIELD(int32, m_movementType)
        SCHEMA_FIELD(CUtlSymbolLarge, m_sMaster)
    };

    class CBaseTrigger : public CBaseToggle
    {
    public:
        DECLARE_SCHEMA_CLASS(CBaseTrigger);
        SCHEMA_FIELD(CEntityIOOutput, m_OnStartTouch);
        SCHEMA_FIELD(CEntityIOOutput, m_OnStartTouchAll);
        SCHEMA_FIELD(CEntityIOOutput, m_OnEndTouch);
        SCHEMA_FIELD(CEntityIOOutput, m_OnEndTouchAll);
        SCHEMA_FIELD(CEntityIOOutput, m_OnTouching);
        SCHEMA_FIELD(CEntityIOOutput, m_OnTouchingEachEntity);
        SCHEMA_FIELD(CEntityIOOutput, m_OnNotTouching);
        SCHEMA_FIELD_POINTER(CUtlVector<CHandle<CBaseEntity>>, m_hTouchingEntities)
        SCHEMA_FIELD(CUtlSymbolLarge, m_iFilterName)
        SCHEMA_FIELD(CEntityHandle, m_hFilter)
        SCHEMA_FIELD(bool, m_bDisabled);
        SCHEMA_FIELD(bool, m_bUseAsyncQueries);
    };

    class CTriggerMultiple : public CBaseTrigger
    {
    public:
        DECLARE_SCHEMA_CLASS(CTriggerMultiple);
        SCHEMA_FIELD(CEntityIOOutput, m_OnTrigger);
    };

    class CBombTarget : public CBaseTrigger
    {
    public:
        DECLARE_SCHEMA_CLASS(CBombTarget);

        SCHEMA_FIELD(bool, m_bIsBombSiteB);
    };
}
