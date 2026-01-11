//
// Created by Michal Přikryl on 23.08.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBaseEntity.h"

namespace TemplatePlugin {
    class CSoundEventEntity : public CBaseEntity {
    public:
        DECLARE_SCHEMA_CLASS(CSoundEventEntity)
        SCHEMA_FIELD(bool, m_bStartOnSpawn);

        SCHEMA_FIELD(bool, m_bToLocalPlayer);

        SCHEMA_FIELD(bool, m_bStopOnNew);

        SCHEMA_FIELD(bool, m_bSaveRestore);

        SCHEMA_FIELD(bool, m_bSavedIsPlaying);

        SCHEMA_FIELD(float32, m_flSavedElapsedTime);

        SCHEMA_FIELD(CUtlSymbolLarge, m_iszSourceEntityName);

        SCHEMA_FIELD(CUtlSymbolLarge, m_iszAttachmentName);

        SCHEMA_FIELD(char, m_onGUIDChanged);

        SCHEMA_FIELD(CEntityIOOutput, m_onSoundFinished);

        SCHEMA_FIELD(float32, m_flClientCullRadius);

        SCHEMA_FIELD(CUtlSymbolLarge, m_iszSoundName);

        SCHEMA_FIELD(CEntityHandle, m_hSource);

        SCHEMA_FIELD(int32, m_nEntityIndexSelection);
    };
}