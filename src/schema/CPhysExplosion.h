//
// Created by Michal Přikryl on 09.11.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBaseEntity.h"
#include "globaltypes.h"

namespace TemplatePlugin
{
    class CPhysExplosion : public CBaseEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CPhysExplosion);

        SCHEMA_FIELD(bool, m_bExplodeOnSpawn)
        SCHEMA_FIELD(float, m_flMagnitude)
        SCHEMA_FIELD(float, m_flDamage)
        SCHEMA_FIELD(float, m_radius)
        SCHEMA_FIELD_POINTER(char, m_targetEntityName)
        SCHEMA_FIELD(float, m_flInnerRadius)
        SCHEMA_FIELD(float, m_flPushScale)
        SCHEMA_FIELD(bool, m_bConvertToDebrisWhenPossible)
        SCHEMA_FIELD(bool, m_bAffectInvulnerableEnts)
        SCHEMA_FIELD(CEntityIOOutput, m_OnPushedPlayer)
    };
}
