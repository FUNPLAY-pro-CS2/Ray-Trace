//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBaseModelEntity.h"
#include "globaltypes.h"

namespace TemplatePlugin {
    class CPlantedC4 : public CBaseModelEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CPlantedC4)

        SCHEMA_FIELD(GameTime_t, m_flC4Blow);
        SCHEMA_FIELD(GameTime_t, m_flDefuseCountDown);
    };
}