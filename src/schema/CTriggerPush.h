//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include "CBaseTrigger.h"
#include "schemasystem.h"

#define SF_TRIG_PUSH_ONCE 0x80

namespace TemplatePlugin {
    class CTriggerPush : public CBaseTrigger
    {
    public:
        DECLARE_SCHEMA_CLASS(CTriggerPush);

        SCHEMA_FIELD(Vector, m_vecPushDirEntitySpace)
        SCHEMA_FIELD(bool, m_bTriggerOnStartTouch)
    };
}