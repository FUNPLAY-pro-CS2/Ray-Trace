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
    class CBaseFilter : public CBaseEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CBaseFilter);

        SCHEMA_FIELD(bool, m_bNegated)
        SCHEMA_FIELD(CEntityIOOutput, m_OnPass)
        SCHEMA_FIELD(CEntityIOOutput, m_OnFail)
    };
}
