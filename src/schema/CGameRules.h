//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <platform.h>
#include "CBaseEntity.h"

namespace TemplatePlugin {
    enum class RoundEndReason : uint32_t
    {
        Unknown = 0x0u,
        TargetBombed = 0x1u,
        TerroristsEscaped = 0x4u,
        CTsPreventEscape = 0x5u,
        EscapingTerroristsNeutralized = 0x6u,
        BombDefused = 0x7u,
        CTsWin = 0x8u,
        TerroristsWin = 0x9u,
        RoundDraw = 0xAu,
        AllHostageRescued = 0xBu,
        TargetSaved = 0xCu,
        HostagesNotRescued = 0xDu,
        TerroristsNotEscaped = 0xEu,
        GameCommencing = 0x10u,

        TerroristsSurrender = 0x11u,
        CTsSurrender = 0x12u,

        TerroristsPlanted = 0x13u,
        CTsReachedHostage = 0x14u,
        SurvivalWin = 0x15u,
        SurvivalDraw = 0x16u,

        TerroristsPlanned = 0x13u
    };

    class CGameRules
    {
    public:
        DECLARE_SCHEMA_CLASS(CGameRules)
    };

    class CCSGameModeRules_Deathmatch : public CGameRules
    {
    public:
        DECLARE_SCHEMA_CLASS(CCSGameModeRules_Deathmatch)

        SCHEMA_FIELD(GameTime_t, m_flDMBonusStartTime);
        SCHEMA_FIELD(float32, m_flDMBonusTimeLength);
        SCHEMA_FIELD(CUtlString, m_sDMBonusWeapon);
    };

    class CCSGameRules;

    class CCSGameRulesProxy : public CBaseEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CCSGameRulesProxy)

        SCHEMA_FIELD(CCSGameRules *, m_pGameRules);
    };

    class CCSGameRules : public CGameRules
    {
    public:
        DECLARE_SCHEMA_CLASS(CCSGameRules)

        SCHEMA_FIELD(float, m_fMatchStartTime);
        SCHEMA_FIELD(float, m_flGameStartTime);
        SCHEMA_FIELD(int, m_totalRoundsPlayed);
        SCHEMA_FIELD(float, m_fRoundStartTime);
        SCHEMA_FIELD(float, m_flRestartRoundTime);
        SCHEMA_FIELD_POINTER(int, m_nEndMatchMapGroupVoteOptions)
        SCHEMA_FIELD(int, m_nEndMatchMapVoteWinner);
        SCHEMA_FIELD(int, m_iRoundTime);
        SCHEMA_FIELD(bool, m_bWarmupPeriod);
        SCHEMA_FIELD(float, m_fWarmupPeriodEnd);
        SCHEMA_FIELD(float, m_fWarmupPeriodStart);
        SCHEMA_FIELD(GamePhase, m_gamePhase);
        SCHEMA_FIELD(bool, m_bGameRestart);
        SCHEMA_FIELD(bool, m_bBombPlanted);
        SCHEMA_FIELD(int32_t, m_totaArenaoundsPlayed);
        SCHEMA_FIELD(int32_t, m_nOvertimePlaying);
        SCHEMA_FIELD(bool, m_bBuyTimeEnded);
        SCHEMA_FIELD(bool, m_bTCantBuy);
        SCHEMA_FIELD(bool, m_bCTCantBuy);
        SCHEMA_FIELD(bool, m_bSwitchingTeamsAtRoundReset);
        SCHEMA_FIELD(int, m_iRoundEndWinnerTeam);
        SCHEMA_FIELD(CUtlString, m_sRoundEndMessage);
        SCHEMA_FIELD(CUtlString, m_sRoundEndFunFactToken);
        SCHEMA_FIELD(bool, m_bIsValveDS);
        SCHEMA_FIELD(bool, m_bIsQuestEligible);
        SCHEMA_FIELD(int32, m_iSpectatorSlotCount);
        SCHEMA_FIELD(float, m_fWarmupNextChatNoticeTime);
        SCHEMA_FIELD_POINTER(CUtlVector<SpawnPoint*>, m_CTSpawnPoints);
        SCHEMA_FIELD_POINTER(CUtlVector<SpawnPoint*>, m_TerroristSpawnPoints);

        using TerminateRoundFn = void(*)(CCSGameRules*, RoundEndReason, float, void*, uint8_t);
        TerminateRoundFn s_pTerminateRound = nullptr;

        void TerminateRound(float delay, RoundEndReason roundEndReason) {
            if (!s_pTerminateRound) {
                TerminateRoundFn addr = DynLibUtils::CModule(shared::g_pServer).FindPattern(
                        shared::g_pGameConfig->GetSignature("CCSGameRules_TerminateRound")).RCast<TerminateRoundFn>();

                if (!addr)
                    return;

                s_pTerminateRound = addr;
            }
            s_pTerminateRound(this, roundEndReason, delay, nullptr, 0);
        }

        static CCSGameRules* FindGameRules()
        {
            auto entities = UTIL_FindAllEntitiesByDesignerName<CCSGameRulesProxy>("cs_gamerules");
            if (entities.empty())
                return nullptr;

            auto* proxy = entities.front();
            return proxy ? proxy->m_pGameRules() : nullptr;
        }
    };
}