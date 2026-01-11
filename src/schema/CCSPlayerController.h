//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <colors.h>
#include <detourtypes.h>
#include <tasks.h>
#include <vectorextends.h>
#include "CBasePlayerController.h"
#include "serversideclient.h"
#include "services.h"
#include "Shared.h"
#include "RayTrace.h"

namespace TemplatePlugin {
    extern CServerSideClient* GetClientBySlot(CPlayerSlot slot);

    class CCSPlayerController : public CBasePlayerController
    {
    public:
        DECLARE_SCHEMA_CLASS(CCSPlayerController);

        SCHEMA_FIELD(CCSPlayerController_InGameMoneyServices*, m_pInGameMoneyServices);

        SCHEMA_FIELD(CCSPlayerController_ActionTrackingServices*, m_pActionTrackingServices);

        SCHEMA_FIELD(CCSPlayerController_InventoryServices*, m_pInventoryServices);

        SCHEMA_FIELD(uint32_t, m_iPing);

        SCHEMA_FIELD(CUtlSymbolLarge, m_szClan);

        SCHEMA_FIELD_POINTER(char, m_szClanName) // char m_szClanName[32]
        SCHEMA_FIELD(bool, m_bEverFullyConnected);

        SCHEMA_FIELD(bool, m_bPawnIsAlive);

        SCHEMA_FIELD(int32_t, m_nDisconnectionTick);

        SCHEMA_FIELD(CHandle<CCSPlayerPawn>, m_hPlayerPawn);

        SCHEMA_FIELD(int32, m_DesiredObserverMode);

        SCHEMA_FIELD(int16_t, m_nPawnCharacterDefIndex);

        SCHEMA_FIELD(CHandle<CCSPlayerPawnBase>, m_hObserverPawn);

        SCHEMA_FIELD(CHandle<CCSPlayerController>, m_hOriginalControllerOfCurrentPawn);

        SCHEMA_FIELD(uint32_t, m_iPawnHealth);

        SCHEMA_FIELD(int32_t, m_iPawnArmor);

        SCHEMA_FIELD(int32_t, m_iScore);

        SCHEMA_FIELD(int32_t, m_iRoundScore);

        SCHEMA_FIELD(int32_t, m_iRoundsWon);

        SCHEMA_FIELD(int32_t, m_iMVPs);

        SCHEMA_FIELD(float, m_flSmoothedPing);

        SCHEMA_FIELD(GameTime_t, m_flForceTeamTime);

        SCHEMA_FIELD(int32_t, m_iCompetitiveRanking);

        SCHEMA_FIELD(int8_t, m_iCompetitiveRankType);

        SCHEMA_FIELD(int32_t, m_iCompetitiveWins);

        SCHEMA_FIELD(byte, m_iPendingTeamNum);

        SCHEMA_FIELD(byte, m_bTeamChanged);

        SCHEMA_FIELD(byte, m_bInSwitchTeam);

        static CCSPlayerController* FromUserId(int userid)
        {
            for (int i = 0; i < shared::getGlobalVars()->maxClients; ++i)
            {
                CCSPlayerController* controller = FromSlot(i);
                if (!controller)
                    continue;

                int iUserId = shared::g_pEngine->GetPlayerUserId(i).Get();
                if (userid == iUserId) return controller;
            }
            return nullptr;
        }

        static CCSPlayerController* FromPawn(CCSPlayerPawn* pawn)
        {
            return reinterpret_cast<CCSPlayerController*>(pawn->m_hController().Get());
        }

        static CCSPlayerController* FromIndex(int iIndex)
        {
            return static_cast<CCSPlayerController*>(shared::g_pEntitySystem->
                GetEntityInstance(CEntityIndex(iIndex)));
        }

        static CCSPlayerController* FromSlot(int iSlot)
        {
            return static_cast<CCSPlayerController*>(shared::g_pEntitySystem->
                GetEntityInstance(CEntityIndex(iSlot + 1)));
        }

        static CCSPlayerController* FromSteamId(uint64 steamid)
        {
            for (int i = 0; i < shared::getGlobalVars()->maxClients; ++i)
            {
                CCSPlayerController* controller = FromSlot(i);
                if (!controller)
                    continue;

                if (steamid == controller->GetSteamID()) return controller;
            }
            return nullptr;
        }

        CServerSideClient* GetServerSideClient()
        {
            return GetClientBySlot(GetPlayerSlot());
        }

        CCSPlayerPawn* GetPlayerPawn()
        {
            if (auto handle = m_hPlayerPawn(); handle.IsValid())
                return handle.Get();
            return nullptr;
        }

        CCSPlayerPawn* GetObserverPawn()
        {
            if (auto handle = m_hObserverPawn(); handle.IsValid())
                return static_cast<CCSPlayerPawn*>(handle.Get());
            return nullptr;
        }

        bool IsBot() { return GetSteamID() == 0; }

        void ChangeTeam(CsTeam iTeam)
        {
            CALL_VIRTUAL(void, shared::g_pGameConfig->GetOffset("CCSPlayerController_ChangeTeam"), this,
                         static_cast<byte>(iTeam));
        }

        static std::string CsTeamToString(CsTeam team)
        {
            switch (team)
            {
            case CsTeam::None: return "None";
            case CsTeam::Spectator: return "Spectator";
            case CsTeam::Terrorist: return "Terrorist";
            case CsTeam::CounterTerrorist: return "CounterTerrorist";
            default: return "Unknown";
            }
        }

        CsTeam Team()
        {
            byte teamNum = GetTeam();
            if (teamNum < static_cast<byte>(CsTeam::None) || teamNum > static_cast<byte>(CsTeam::CounterTerrorist))
                return CsTeam::None;

            return static_cast<CsTeam>(teamNum);
        }

        void Respawn()
        {
            CCSPlayerPawn* pPawn = GetPlayerPawn();
            if (!pPawn || m_bPawnIsAlive)
                return;

            SetPawn(pPawn);
            CALL_VIRTUAL(void, shared::g_pGameConfig->GetOffset("CCSPlayerController_Respawn"), this);
        }

        bool CheckValid(bool alive = false)
        {
            CCSPlayerPawn* pawn = GetPlayerPawn();

            if (IsBot() || GetIsHLTV() || GetSteamID() <= 0 || !IsConnected() || !pawn)
                return false;

            if (alive && !m_bPawnIsAlive())
            {
                return false;
            }

            return true;
        }

        void TakeFakeDamage(CCSPlayerController* attacker, int damage)
        {
            static CBaseEntity_TakeDamageOld_t s_TakeDamageOld = nullptr;

            if (!s_TakeDamageOld)
            {
                CBaseEntity_TakeDamageOld_t addr =
                    DynLibUtils::CModule(shared::g_pServer)
                    .FindPattern(shared::g_pGameConfig->GetSignature("CBaseEntity_TakeDamageOld"))
                    .RCast<CBaseEntity_TakeDamageOld_t>();

                if (!addr) return;
                s_TakeDamageOld = addr;
            }

            if (!CheckValid(true) || !attacker || !attacker->CheckValid())
                return;

            auto* victimPawn = GetPlayerPawn();
            if (!victimPawn) return;

            auto* attackerPawn = attacker->GetPlayerPawn();
            if (!attackerPawn) return;

            const size_t infoSize = schema::GetClassSize("CTakeDamageInfo");
            const size_t resultSize = schema::GetClassSize("CTakeDamageResult");

            auto* infoMem = std::malloc(infoSize);
            auto* resultMem = std::malloc(resultSize);

            if (!infoMem || !resultMem)
            {
                std::free(infoMem);
                std::free(resultMem);
                return;
            }

            std::memset(infoMem, 0, infoSize);
            std::memset(resultMem, 0, resultSize);

            CAttackerInfo attackerInfo(attacker);
            std::memcpy(reinterpret_cast<std::uint8_t*>(infoMem) + 0x98, &attackerInfo, sizeof(CAttackerInfo));

            auto* info = reinterpret_cast<CTakeDamageInfo*>(infoMem);
            auto* result = reinterpret_cast<CTakeDamageResult*>(resultMem);

            info->m_hInflictor() = attackerPawn->GetHandle();
            info->m_hAttacker() = attackerPawn->GetHandle();
            info->m_flDamage() = static_cast<float>(damage);
            info->m_flFriendlyFireDamageReductionRatio() = static_cast<float>(0);
            info->m_bInTakeDamageFlow() = true;
            info->m_bitsDamageType() = DMG_GENERIC;

            result->m_pOriginatingInfo() = info;
            result->m_nDamageDealt() = damage;
            result->m_nHealthLost() = damage;
            result->m_flPreModifiedDamage() = static_cast<float>(damage);
            result->m_nTotalledDamageDealt() = damage;
            result->m_nTotalledHealthLost() = damage;
            result->m_bWasDamageSuppressed() = false;

            s_TakeDamageOld(victimPawn, info, result);

            std::free(infoMem);
            std::free(resultMem);
        }

        void Kick(ENetworkDisconnectionReason reason)
        {
            if (!CheckValid()) return;

            CPlayerSlot slot = CPlayerSlot(GetPlayerSlot());

            Tasks::NextFrame([slot, reason]
            {
                shared::g_pEngine->DisconnectClient(
                    slot, reason);
            });
        }

        PlayerConnectedState GetConnectedState() { return m_iConnected(); }

        CSPlayerState GetPawnState()
        {
            CCSPlayerPawnBase* pPawn = static_cast<CCSPlayerPawnBase*>(GetPawn());
            if (!pPawn)
                return STATE_WELCOME;

            return pPawn->m_iPlayerState();
        }

        CSPlayerState GetPlayerPawnState()
        {
            CCSPlayerPawn* pPawn = GetPlayerPawn();
            if (!pPawn)
                return STATE_WELCOME;

            return pPawn->m_iPlayerState();
        }

        CBaseEntity* GetObserverTarget()
        {
            auto pPawn = GetPawn();

            if (!pPawn)
                return nullptr;

            return pPawn->m_pObserverServices->m_hObserverTarget().Get();
        }

        // --- HEALTH & ARMOR ---
        int GetHealth()
        {
            if (!CheckValid(true)) return 0;
            auto pawn = GetPlayerPawn();
            return pawn->m_iHealth;
        }

        void SetHealth(int health)
        {
            if (!CheckValid(true) || health <= 0) return;
            auto pawn = GetPlayerPawn();
            pawn->m_iHealth = health;
            m_iPawnHealth = health;
            if (health > pawn->m_iMaxHealth)
                pawn->m_iMaxHealth = health;
        }

        void AddHealth(int delta)
        {
            if (!CheckValid(true) || delta <= 0) return;
            auto pawn = GetPlayerPawn();
            pawn->m_iHealth += delta;
            m_iPawnHealth += delta;
        }

        void SetMaxHealth(int maxHp)
        {
            if (!CheckValid(true) || maxHp <= 0) return;
            auto pawn = GetPlayerPawn();
            pawn->m_iMaxHealth = maxHp;
        }

        void SetArmor(int armor, bool helmet = false)
        {
            if (!CheckValid(true) || armor < 0) return;
            auto pawn = GetPlayerPawn();
            pawn->m_ArmorValue = armor;
            m_iPawnArmor = armor;
            if (helmet && pawn->m_pItemServices)
                pawn->m_pItemServices->m_bHasHelmet = true;
        }

        void AddArmor(int delta)
        {
            if (!CheckValid(true) || delta <= 0) return;
            auto pawn = GetPlayerPawn();
            pawn->m_ArmorValue() += delta;
            m_iPawnArmor() += delta;
        }

        // --- WEAPONS ---
        std::vector<CCSWeaponBase*> GetWeapons()
        {
            std::vector<CCSWeaponBase*> weapons;
            auto pawn = GetPlayerPawn();
            if (!pawn || !pawn->m_pWeaponServices() || !pawn->m_pWeaponServices()->m_hMyWeapons())
                return weapons;

            const auto& myWeapons = *pawn->m_pWeaponServices()->m_hMyWeapons();

            for (int i = 0; i < myWeapons.Count(); ++i)
            {
                const auto& handle = myWeapons[i];
                if (!handle.IsValid()) continue;

                if (auto* weapon = static_cast<CCSWeaponBase*>(handle.Get()))
                {
                    weapons.push_back(weapon);
                }
            }

            return weapons;
        }

        CCSWeaponBase* GetWeaponByDefIndex(int defIndex)
        {
            for (auto* base : GetWeapons())
            {
                if (base->m_AttributeManager().m_Item().m_iItemDefinitionIndex() == defIndex)
                    return base;
            }
            return nullptr;
        }

        CCSWeaponBase* GetWeaponBySlot(gear_slot_t slot)
        {
            for (auto* base : GetWeapons())
            {
                if (base->GetWeaponVData() && base->GetWeaponVData()->m_GearSlot() == slot)
                    return base;
            }
            return nullptr;
        }

        CCSWeaponBase* GetWeaponByName(const std::string& weaponName)
        {
            for (auto* base : GetWeapons())
            {
                if (base->GetDesignerName() == weaponName)
                    return base;
            }
            return nullptr;
        }

        bool HasWeaponByName(const std::string& weaponName)
        {
            return GetWeaponByName(weaponName) != nullptr;
        }

        bool HasWeaponInSlot(gear_slot_t slot)
        {
            return GetWeaponBySlot(slot) != nullptr;
        }

        std::string GetWeaponNameBySlot(gear_slot_t slot)
        {
            if (auto* w = GetWeaponBySlot(slot))
            {
                return w->GetDesignerName();
            }
            return {};
        }

        void DropWeaponByName(const std::string& weaponName)
        {
            auto pawn = GetPlayerPawn();
            if (!pawn || !pawn->m_pWeaponServices()) return;

            if (auto* weapon = GetWeaponByName(weaponName))
            {
                pawn->m_pWeaponServices()->m_hActiveWeapon() = CHandle<CBaseEntity>(weapon);
                pawn->m_pItemServices()->DropActiveWeapon(weapon);
            }
        }

        // --- POSITION / AIM ---
        Vector GetEyePosition()
        {
            auto pawn = GetPlayerPawn();
            if (!pawn) return {0, 0, 0};
            Vector origin = pawn->GetAbsOrigin();
            Vector offset = pawn->m_vecViewOffset();
            return {origin.x, origin.y, origin.z + offset.z};
        }

        CBaseEntity* GetAimEntity(bool includePlayers = false)
        {
            if (!CheckValid(true))
                return nullptr;

            auto* pawn = GetPlayerPawn();
            if (!pawn)
                return nullptr;

            QAngle eyeAngles = pawn->GetEyeAngles();
            Vector eyePos = GetEyePosition();

            RayTrace::TraceOptions opts{};
            if (!includePlayers)
                opts.InteractsExclude = RayTrace::InteractionLayers::Player;

            auto result = RayTrace::TraceShape(
                eyePos,
                eyeAngles,
                pawn,
                &opts
            );

            return result.has_value()
                       ? static_cast<CBaseEntity*>(result->HitEntity)
                       : nullptr;
        }

        struct AimPositionResult
        {
            bool Hit{false};
            Vector Origin{};
            Vector Normal{};
        };

        AimPositionResult GetAimPositionEx(bool includePlayers = true)
        {
            AimPositionResult result{};

            if (!CheckValid(true))
                return result;

            auto* pawn = GetPlayerPawn();
            if (!pawn)
                return result;

            QAngle eyeAngles = pawn->GetEyeAngles();
            Vector eyePos = GetEyePosition();

            Vector forward{};
            AngleVectors(eyeAngles, &forward, nullptr, nullptr);

            Vector origin = eyePos + forward * 15.0f;

            RayTrace::TraceOptions opts{};
            if (!includePlayers)
                opts.InteractsExclude = RayTrace::InteractionLayers::Player;

            auto traceResult = RayTrace::TraceShape(
                eyePos,
                eyeAngles,
                pawn,
                &opts
            );

            if (traceResult.has_value())
            {
                result.Hit = true;
                result.Origin = traceResult->EndPos;
                result.Normal = traceResult->Normal;
                return result;
            }

            result.Hit = false;
            result.Origin = origin + forward * 8192.0f;
            return result;
        }

        Vector GetAimPosition(bool includePlayers = true)
        {
            if (!CheckValid(true))
                return {0, 0, 0};

            auto* pawn = GetPlayerPawn();
            if (!pawn)
                return {0, 0, 0};

            QAngle eyeAngles = pawn->GetEyeAngles();
            Vector eyePos = GetEyePosition();

            Vector forward{};
            AngleVectors(eyeAngles, &forward, nullptr, nullptr);

            Vector origin = eyePos + forward * 15.0f;

            RayTrace::TraceOptions opts{};
            if (!includePlayers)
                opts.InteractsExclude = RayTrace::InteractionLayers::Player;

            auto traceResult = RayTrace::TraceShape(
                eyePos,
                eyeAngles,
                pawn,
                &opts
            );

            if (traceResult.has_value())
            {
                return traceResult->EndPos;
            }

            return origin + forward * 8192.0f;
        }

        bool IsPlayerInSightRange(CCSPlayerController* target,
                                  float distance = 0.0f, float angle = 90.0f, bool heightCheck = true,
                                  bool negativeAngle = false)
        {
            if (!CheckValid(true) || !target->CheckValid(true) || GetPlayerPawn()->GetAbsOrigin().IsZero() ||
                target->GetPlayerPawn()->GetAbsOrigin().IsZero())
                return false;

            float resultDistance = 0;
            auto playerEyeAngles = GetPlayerPawn()->GetEyeAngles();
            playerEyeAngles.x = 0.0f;
            playerEyeAngles.z = 0.0f;
            Vector angleVector;
            AngleVectors(playerEyeAngles, &angleVector, nullptr, nullptr);
            angleVector = VectorExtends::Normalize(angleVector);

            if (negativeAngle)
                angleVector = VectorExtends::NegateVector(angleVector);

            auto playerOrigin = GetPlayerPawn()->GetAbsOrigin();
            auto targetOrigin = target->GetPlayerPawn()->GetAbsOrigin();

            if (heightCheck && distance > 0.0f)
                resultDistance = VectorExtends::Distance(playerOrigin, targetOrigin);

            if (distance > 0.0f)
            {
                if (resultDistance > distance)
                    return false;
            }

            playerOrigin.z = 0.0f;
            targetOrigin.z = 0.0f;

            auto targetVector = VectorExtends::MakeVectorFromPoints(playerOrigin, targetOrigin);
            targetVector = VectorExtends::Normalize(targetVector);
            auto resultAngle = VectorExtends::RadToDeg(
                VectorExtends::ArcCosine(VectorExtends::GetVectorDotProduct(targetVector, angleVector)));

            if (resultAngle > angle / 2)
                return false;

            if (distance > 0.0f)
            {
                if (!heightCheck)
                    resultDistance = VectorExtends::Distance(playerOrigin, targetOrigin);
                return resultDistance <= distance;
            }

            return true;
        }

        void TeleportTo(CCSPlayerController* target)
        {
            if (!CheckValid(true) || !target || !target->CheckValid(true)) return;

            auto pawn = GetPlayerPawn();
            auto tpawn = target->GetPlayerPawn();
            if (!pawn || !tpawn) return;

            Vector origin = tpawn->GetAbsOrigin();
            QAngle rot = tpawn->GetAbsRotation();
            Vector vel = tpawn->GetAbsVelocity();

            pawn->Teleport(&origin, &rot, &vel);
        }

        void TeleportPlayer(CCSPlayerController* target)
        {
            CCSPlayerPawn* myPawn = GetPlayerPawn();
            CCSPlayerPawn* targetPawn = target ? target->GetPlayerPawn() : nullptr;

            if (!myPawn || !targetPawn)
                return;

            const Vector& origin = targetPawn->GetAbsOrigin();
            const QAngle& rotation = targetPawn->GetAbsRotation();
            const Vector& velocity = targetPawn->GetAbsVelocity();

            myPawn->Teleport(&origin, &rotation, &velocity);
        }

        void SetPlayerRenderColor(int a = 255, int r = 255, int g = 255, int b = 255)
        {
            CCSPlayerPawn* pawn = GetPlayerPawn();
            if (!pawn) return;

            pawn->m_clrRender() = Color(r, g, b, a);
        }

        void SetPlayerInvisible(int alpha = 0)
        {
            if (alpha < 0) alpha = 0;
            if (alpha > 255) alpha = 255;

            CCSPlayerPawn* pawn = GetPlayerPawn();
            if (!pawn)
                return;

            SetPlayerRenderColor(alpha);

            if (auto weaponServices = pawn->m_pWeaponServices())
            {
                CHandle<CBasePlayerWeapon> activeWeapon = weaponServices->m_hActiveWeapon();
                if (activeWeapon && activeWeapon.IsValid())
                {
                    auto* weapon = activeWeapon.Get();
                    weapon->m_clrRender() = colors(255, 255, 255, alpha).ToValveColor();
                    weapon->m_flShadowStrength() = 0.0f;
                }

                if (auto myWeapons = weaponServices->m_hMyWeapons())
                {
                    FOR_EACH_VEC(*myWeapons, i)
                    {
                        const CHandle<CBasePlayerWeapon>& handle = (*myWeapons)[i];
                        if (!handle.IsValid() || !handle.Get())
                            continue;

                        auto* weapon = handle.Get();
                        weapon->m_clrRender() = colors(255, 255, 255, 0).ToValveColor();
                        weapon->m_flShadowStrength() = 0.0f;
                    }
                }
            }
        }

        void SetPlayerVisible()
        {
            CCSPlayerPawn* pawn = GetPlayerPawn();
            if (!pawn)
                return;

            SetPlayerRenderColor();

            if (auto weaponServices = pawn->m_pWeaponServices())
            {
                CHandle<CBasePlayerWeapon> activeWeapon = weaponServices->m_hActiveWeapon();
                if (activeWeapon && activeWeapon.IsValid())
                {
                    auto* weapon = activeWeapon.Get();
                    weapon->m_clrRender() = colors(255, 255, 255, 255).ToValveColor();
                    weapon->m_flShadowStrength() = 1.0f;
                }

                if (auto myWeapons = weaponServices->m_hMyWeapons())
                {
                    FOR_EACH_VEC(*myWeapons, i)
                    {
                        const CHandle<CBasePlayerWeapon>& handle = (*myWeapons)[i];
                        if (!handle.IsValid() || !handle.Get())
                            continue;

                        auto* weapon = handle.Get();
                        weapon->m_clrRender() = colors(255, 255, 255, 255).ToValveColor();
                        weapon->m_flShadowStrength() = 1.0f;
                    }
                }
            }
        }
    };
}
