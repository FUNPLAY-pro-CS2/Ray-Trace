//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include "CBaseEntity.h"
#include"schemasystem.h"

#define SF_TRIG_PUSH_ONCE 0x80

namespace TemplatePlugin {
    class CEnvEntityMaker : public CBaseEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CEnvEntityMaker);

        SCHEMA_FIELD(CUtlSymbolLarge, m_iszTemplate)
    };
}