//
// Created by Michal Přikryl on 31.10.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBaseEntity.h"
#include "globaltypes.h"
#include "CBaseAnimGraph.h"

namespace
TemplatePlugin
{
    class CBaseProp : public CBaseAnimGraph
    {
    public:
        DECLARE_SCHEMA_CLASS(CBaseProp)

        SCHEMA_FIELD(bool, m_bModelOverrodeBlockLOS)
        SCHEMA_FIELD(int, m_iShapeType)
        SCHEMA_FIELD(bool, m_bConformToCollisionBounds)
        SCHEMA_FIELD(CTransform, m_mPreferredCatchTransform)
    };
}
