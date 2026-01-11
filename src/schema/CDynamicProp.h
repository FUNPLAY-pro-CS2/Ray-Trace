//
// Created by Michal Přikryl on 01.11.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "CBaseEntity.h"
#include "globaltypes.h"
#include "CBreakableProp.h"

namespace
TemplatePlugin
{
    enum AnimLoopMode_t : uint
    {
        ANIM_LOOP_MODE_INVALID = 0xFFFFFFFF,
        ANIM_LOOP_MODE_NOT_LOOPING = 0x0,
        ANIM_LOOP_MODE_LOOPING = 0x1,
        ANIM_LOOP_MODE_USE_SEQUENCE_SETTINGS = 0x2,
        ANIM_LOOP_MODE_COUNT = 0x3,
    };

    class CDynamicProp : public CBreakableProp
    {
    public:
        DECLARE_SCHEMA_CLASS(CDynamicProp);

        SCHEMA_FIELD(bool, m_bCreateNavObstacle)
        SCHEMA_FIELD(bool, m_bNavObstacleUpdatesOverridden)
        SCHEMA_FIELD(bool, m_bUseHitboxesForRenderBox)
        SCHEMA_FIELD(bool, m_bUseAnimGraph)
        SCHEMA_FIELD(CEntityIOOutput, m_pOutputAnimBegun)
        SCHEMA_FIELD(CEntityIOOutput, m_pOutputAnimOver)
        SCHEMA_FIELD(CEntityIOOutput, m_pOutputAnimLoopCycleOver)
        SCHEMA_FIELD(CEntityIOOutput, m_OnAnimReachedStart)
        SCHEMA_FIELD(CEntityIOOutput, m_OnAnimReachedEnd)
        SCHEMA_FIELD_POINTER(char, m_iszIdleAnim)
        SCHEMA_FIELD(AnimLoopMode_t, m_nIdleAnimLoopMode)
        SCHEMA_FIELD(bool, m_bRandomizeCycle)
        SCHEMA_FIELD(bool, m_bStartDisabled)
        SCHEMA_FIELD(bool, m_bFiredStartEndOutput)
        SCHEMA_FIELD(bool, m_bForceNpcExclude)
        SCHEMA_FIELD(bool, m_bCreateNonSolid)
        SCHEMA_FIELD(bool, m_bIsOverrideProp)
        SCHEMA_FIELD(int32, m_iInitialGlowState)
        SCHEMA_FIELD(int32, m_nGlowRange)
        SCHEMA_FIELD(int32, m_nGlowRangeMin)
        SCHEMA_FIELD(Color, m_glowColor)
        SCHEMA_FIELD(int32, m_nGlowTeam)
    };
}
