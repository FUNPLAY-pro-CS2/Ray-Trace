//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBaseEntity.h"

namespace TemplatePlugin {
    class CTeam : public CBaseEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CTeam)

        SCHEMA_FIELD(int32_t, m_iScore);
    };
}