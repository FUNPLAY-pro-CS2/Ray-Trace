//
// Created by Michal Přikryl on 07.11.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBaseTrigger.h"
#include "globaltypes.h"

namespace
TemplatePlugin
{
    class locksound_t
    {
    public:
        DECLARE_SCHEMA_CLASS(locksound_t);

        SCHEMA_FIELD_POINTER(char, sLockedSound)
        SCHEMA_FIELD_POINTER(char, sUnlockedSound)
        SCHEMA_FIELD(float, flwaitSound)
    };

    class CBaseButton : public CBaseToggle
    {
    public:
        DECLARE_SCHEMA_CLASS(CBaseButton);

        SCHEMA_FIELD(QAngle, m_angMoveEntitySpace)
        SCHEMA_FIELD(bool, m_fStayPushed)
        SCHEMA_FIELD(bool, m_fRotating)
        SCHEMA_FIELD(locksound_t, m_ls)
        SCHEMA_FIELD_POINTER(char, m_sUseSound)
        SCHEMA_FIELD_POINTER(char, m_sLockedSound)
        SCHEMA_FIELD_POINTER(char, m_sUnlockedSound)
        SCHEMA_FIELD_POINTER(char, m_sOverrideAnticipationName)
        SCHEMA_FIELD(bool, m_bLocked)
        SCHEMA_FIELD(bool, m_bDisabled)
        SCHEMA_FIELD(float, m_flUseLockedTime)
        SCHEMA_FIELD(bool, m_bSolidBsp)
        SCHEMA_FIELD(CEntityIOOutput, m_OnDamaged)
        SCHEMA_FIELD(CEntityIOOutput, m_OnPressed)
        SCHEMA_FIELD(CEntityIOOutput, m_OnUseLocked)
        SCHEMA_FIELD(CEntityIOOutput, m_OnIn)
        SCHEMA_FIELD(CEntityIOOutput, m_OnOut)
        SCHEMA_FIELD(int32, m_nState)
        SCHEMA_FIELD(CHandle<CEntityInstance>, m_hConstraint)
        SCHEMA_FIELD(CHandle<CEntityInstance>, m_hConstraintParent)
        SCHEMA_FIELD(bool, m_bForceNpcExclude)
        SCHEMA_FIELD_POINTER(char, m_sGlowEntity)
        SCHEMA_FIELD(CHandle<CBaseModelEntity>, m_glowEntity)
        SCHEMA_FIELD(bool, m_usable)
        SCHEMA_FIELD_POINTER(char, m_szDisplayText)
    };
}
