//
// Created by Michal Přikryl on 07.11.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
#pragma once
#include "CBaseModelEntity.h"
#include "globaltypes.h"

namespace
TemplatePlugin
{
    class CFuncVPhysicsClip : public CBaseModelEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CFuncVPhysicsClip);
        SCHEMA_FIELD(int, m_bDisabled)
    };
}
