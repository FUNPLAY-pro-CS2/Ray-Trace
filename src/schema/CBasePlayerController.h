//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "ehandle.h"
#include "CCSPlayerPawn.h"

namespace TemplatePlugin
{
    enum class PlayerConnectedState : uint32_t
    {
        PlayerNeverConnected = 0xFFFFFFFF,
        PlayerConnected = 0x0,
        PlayerConnecting = 0x1,
        PlayerReconnecting = 0x2,
        PlayerDisconnecting = 0x3,
        PlayerDisconnected = 0x4,
        PlayerReserved = 0x5,
    };

    class CBasePlayerController : public CBaseEntity
    {
    public:
        DECLARE_SCHEMA_CLASS(CBasePlayerController);

        SCHEMA_FIELD(CHandle<CBasePlayerPawn>, m_hPawn)

        SCHEMA_FIELD_POINTER(char, m_iszPlayerName)

        SCHEMA_FIELD(PlayerConnectedState, m_iConnected)

        SCHEMA_FIELD(uint32_t, m_iDesiredFOV);

        SCHEMA_FIELD_POINTER(uint64_t, m_steamID)

        SCHEMA_FIELD_POINTER(bool, m_bIsHLTV)

        CBasePlayerPawn* GetPawn() { return m_hPawn.Get(); }
        const char* GetPlayerName() { return m_iszPlayerName(); }

        void SetPlayerName(const std::string& name)
        {
            std::strncpy(m_iszPlayerName(), name.c_str(), 128);
            m_iszPlayerName()[127] = '\0';
        }

        int GetPlayerSlot() { return GetEntityIndex().Get() - 1; }
        bool IsConnected() { return m_iConnected() == PlayerConnectedState::PlayerConnected; }

        uint64_t GetSteamID() { return *m_steamID(); }
        void SetSteamID(uint64_t id) { *m_steamID() = id; }

        bool GetIsHLTV() { return *m_bIsHLTV(); }
        void SetIsHLTV(bool value) { *m_bIsHLTV() = value; }

        uint64_t& SteamID() { return *m_steamID(); }
        bool& IsHLTV() { return *m_bIsHLTV(); }

        using CBasePlayerController_SetPawn_t = void(*)(CBasePlayerController* pController, CCSPlayerPawn* pPawn,
                                                        bool a3, bool a4, bool a5, bool a6);

        void SetPawn(CCSPlayerPawn* pawn)
        {
            if (!pawn) return;

            CBasePlayerController_SetPawn_t CBasePlayerController_SetPawn = nullptr;

            if (!CBasePlayerController_SetPawn)
            {
                CBasePlayerController_SetPawn_t addr = addr =
                    DynLibUtils::CModule(shared::g_pServer)
                    .FindPattern(shared::g_pGameConfig->GetSignature("CBasePlayerController_SetPawn"))
                    .RCast<CBasePlayerController_SetPawn_t>();
                if (!addr)
                    return;
                CBasePlayerController_SetPawn = addr;
            }

            CBasePlayerController_SetPawn(this, pawn, true, false, false, false);
        }
    };
}
