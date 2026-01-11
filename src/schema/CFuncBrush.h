//
// Created by Michal Přikryl on 07.11.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
#pragma once
#include "CBaseModelEntity.h"
#include "globaltypes.h"

namespace
TemplatePlugin
{
    enum BrushSolidities_e : uint
    {
        BRUSHSOLID_TOGGLE,
        BRUSHSOLID_NEVER,
        BRUSHSOLID_ALWAYS,
    };

    class CFuncBrush : public CBaseModelEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CFuncBrush);
        SCHEMA_FIELD(BrushSolidities_e, m_iSolidity)
        SCHEMA_FIELD(int, m_iDisabled)
        SCHEMA_FIELD(bool, m_bSolidBsp)
        SCHEMA_FIELD_POINTER(char, m_iszExcludedClass)
        SCHEMA_FIELD(bool, m_bInvertExclusion)
        SCHEMA_FIELD(bool, m_bScriptedMovement)
    };
}
