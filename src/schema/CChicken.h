//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBaseEntity.h"

namespace TemplatePlugin {
    enum class ChickenActivity : uint32_t {
        IDLE = 0x0,
        SQUAT = 0x1,
        WALK = 0x2,
        RUN = 0x3,
        GLIDE = 0x4,
        LAND = 0x5,
        PANIC = 0x6
    };

    class CChicken : public CBaseEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CChicken)

        SCHEMA_FIELD(ChickenActivity, m_currentActivity)
    };
}