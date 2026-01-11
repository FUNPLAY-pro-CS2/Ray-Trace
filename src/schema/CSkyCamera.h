//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBaseEntity.h"
#include "globaltypes.h"
#include "CBaseModelEntity.h"

namespace TemplatePlugin {
    class CSkyCamera : public CBaseEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CSkyCamera)
        SCHEMA_FIELD(CUtlStringToken, m_skyboxSlotToken);
    };

    class CEnvSky : public CBaseModelEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CEnvSky)

        SCHEMA_FIELD(int, m_hSkyMaterial)
        SCHEMA_FIELD(int, m_hSkyMaterialLightingOnly)
        SCHEMA_FIELD(Color, m_vTintColor)
        SCHEMA_FIELD(float32, m_flBrightnessScale)
    };
}