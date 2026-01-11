//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <entity2/entityidentity.h>
#include "CGameRules.h"
#include "schemasystem.h"

namespace TemplatePlugin {
    class CC4
    {
    public:
        DECLARE_SCHEMA_CLASS(CC4);

        SCHEMA_FIELD(GameTime_t, m_fArmedTime);
    };
}
