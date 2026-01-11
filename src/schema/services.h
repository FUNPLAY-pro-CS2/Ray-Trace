/**
 * =============================================================================
 * CS2Fixes
 * Copyright (C) 2023-2025 Source2ZE
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "globaltypes.h"
#include "CCSWeaponBase.h"
#include <platform.h>
#include <unordered_map>

#include "schemasystem.h"

#define AMMO_OFFSET_HEGRENADE 13
#define AMMO_OFFSET_FLASHBANG 14
#define AMMO_OFFSET_SMOKEGRENADE 15
#define AMMO_OFFSET_MOLOTOV 16
#define AMMO_OFFSET_DECOY 17

namespace RayTracePlugin {
    class CCSPlayerController;
    class CCSPlayerPawn;
    extern bool g_bAwsChangingTeam;

    struct CSPerRoundStats_t {
        DECLARE_SCHEMA_CLASS_INLINE(CSPerRoundStats_t)

        SCHEMA_FIELD(int32_t, m_iKills);
        SCHEMA_FIELD(int32_t, m_iDeaths);
        SCHEMA_FIELD(int32_t, m_iAssists);
        SCHEMA_FIELD(int32_t, m_iDamage);
        SCHEMA_FIELD(int32_t, m_iHeadShotKills);
        SCHEMA_FIELD(int32_t, m_iUtilityDamage);
        SCHEMA_FIELD(int32_t, m_iEnemiesFlashed);
        SCHEMA_FIELD(int32_t, m_iObjective);
        SCHEMA_FIELD(int32_t, m_iCashEarned);
        SCHEMA_FIELD(int32_t, m_iEquipmentValue);
        SCHEMA_FIELD(int32_t, m_iKillReward);
        SCHEMA_FIELD(int32_t, m_iMoneySaved);
    };

    struct CSMatchStats_t : public CSPerRoundStats_t {
    public:
        DECLARE_SCHEMA_CLASS_INLINE(CSMatchStats_t)

        SCHEMA_FIELD(int32_t, m_iEntryWins);
    };

    class CPlayerControllerComponent {
        virtual ~CPlayerControllerComponent() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CPlayerControllerComponent)

        CCSPlayerController* __m_pChainEntity;
        CCSPlayerController* GetController() { return __m_pChainEntity; }
    };

    class CPlayerPawnComponent {
        virtual ~CPlayerPawnComponent() = 0;

        virtual void unk_01() = 0;

        virtual void unk_02() = 0;

        virtual void unk_03() = 0;

        virtual void unk_04() = 0;

        virtual void unk_05() = 0;

        virtual void unk_06() = 0;

        virtual void unk_07() = 0;

        virtual void unk_08() = 0;

        virtual void unk_09() = 0;

        virtual void unk_10() = 0;

        virtual void unk_11() = 0;

        virtual void unk_12() = 0;

        virtual void unk_13() = 0;

        virtual void unk_14() = 0;

        virtual void unk_15() = 0;

        virtual void unk_16() = 0;

        virtual void unk_17() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CPlayerPawnComponent);

        SCHEMA_FIELD(CCSPlayerPawn*, __m_pChainEntity)

        CCSPlayerPawn *GetPawn() { return __m_pChainEntity; }
    };

    class CPlayer_MovementServices : public CPlayerPawnComponent {
        virtual ~CPlayer_MovementServices() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CPlayer_MovementServices);

        SCHEMA_FIELD(CInButtonState, m_nButtons)

        SCHEMA_FIELD(uint64_t, m_nQueuedButtonDownMask)

        SCHEMA_FIELD(uint64_t, m_nQueuedButtonChangeMask)

        SCHEMA_FIELD(uint64_t, m_nButtonDoublePressed)

        // m_pButtonPressedCmdNumber[64]
        SCHEMA_FIELD_POINTER(uint32_t, m_pButtonPressedCmdNumber)

        SCHEMA_FIELD(uint32_t, m_nLastCommandNumberProcessed)

        SCHEMA_FIELD(uint64_t, m_nToggleButtonDownMask)

        SCHEMA_FIELD(float, m_flMaxspeed)
    };

    class CPlayer_MovementServices_Humanoid : public CPlayer_MovementServices {
        virtual ~CPlayer_MovementServices_Humanoid() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CPlayer_MovementServices_Humanoid);

        SCHEMA_FIELD(float, m_flFallVelocity)

        SCHEMA_FIELD(float, m_bInCrouch)

        SCHEMA_FIELD(uint32_t, m_nCrouchState)

        SCHEMA_FIELD(bool, m_bInDuckJump)

        SCHEMA_FIELD(float, m_flSurfaceFriction)
    };

    class CCSPlayer_MovementServices : public CPlayer_MovementServices_Humanoid {
        virtual ~CCSPlayer_MovementServices() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CCSPlayer_MovementServices);

        SCHEMA_FIELD(float, m_flMaxFallVelocity)

        SCHEMA_FIELD(float, m_flJumpVel)

        SCHEMA_FIELD(float, m_flJumpPressedTime)

        SCHEMA_FIELD(float, m_flStamina)

        SCHEMA_FIELD(float, m_flDuckSpeed)

        SCHEMA_FIELD(bool, m_bDuckOverride)
    };

    class CPlayer_WeaponServices : public CPlayerPawnComponent {
        virtual ~CPlayer_WeaponServices() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CPlayer_WeaponServices);

        SCHEMA_FIELD_POINTER(CUtlVector<CHandle<CBasePlayerWeapon>>, m_hMyWeapons)

        SCHEMA_FIELD(CHandle<CBasePlayerWeapon>, m_hActiveWeapon)

        SCHEMA_FIELD_POINTER(uint16_t, m_iAmmo)
    };

    class CCSPlayer_WeaponServices : public CPlayer_WeaponServices {
        virtual ~CCSPlayer_WeaponServices() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CCSPlayer_WeaponServices);

        SCHEMA_FIELD(GameTime_t, m_flNextAttack)

        SCHEMA_FIELD(bool, m_bIsLookingAtWeapon)

        SCHEMA_FIELD(bool, m_bIsHoldingLookAtWeapon)

        SCHEMA_FIELD(CHandle<CBasePlayerWeapon>, m_hSavedWeapon)

        SCHEMA_FIELD(int32_t, m_nTimeToMelee)

        SCHEMA_FIELD(int32_t, m_nTimeToSecondary)

        SCHEMA_FIELD(int32_t, m_nTimeToPrimary)

        SCHEMA_FIELD(int32_t, m_nTimeToSniperRifle)

        SCHEMA_FIELD(bool, m_bIsBeingGivenItem)

        SCHEMA_FIELD(bool, m_bIsPickingUpItemWithUse)

        SCHEMA_FIELD(bool, m_bPickedUpWeapon)
    };

    class CCSPlayerController_ActionTrackingServices : public CPlayerControllerComponent {
        virtual ~CCSPlayerController_ActionTrackingServices() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CCSPlayerController_ActionTrackingServices)

        SCHEMA_FIELD(CSMatchStats_t, m_matchStats)
        SCHEMA_FIELD(int, m_iNumRoundKills);
    };

    class CCSPlayerController_InGameMoneyServices : public CPlayerControllerComponent {
        virtual ~CCSPlayerController_InGameMoneyServices() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CCSPlayerController_InGameMoneyServices);

        SCHEMA_FIELD(bool, m_bReceivesMoneyNextRound);   // ReceivesMoneyNextRound
        SCHEMA_FIELD(int,  m_iMoneyEarnedForNextRound);  // MoneyEarnedForNextRound
        SCHEMA_FIELD(int,  m_iAccount);                  // Account
        SCHEMA_FIELD(int,  m_iStartAccount);             // StartAccount
        SCHEMA_FIELD(int,  m_iTotalCashSpent);           // TotalCashSpent
        SCHEMA_FIELD(int,  m_iCashSpentThisRound);       // CashSpentThisRound
    };

    class CCSPlayerController_InventoryServices : public CPlayerControllerComponent {
        virtual ~CCSPlayerController_InventoryServices() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CCSPlayerController_InventoryServices);

        SCHEMA_FIELD(int32_t, m_nPersonaDataXpTrailLevel)
        SCHEMA_FIELD(int32_t, m_nPersonaDataPublicLevel)
        SCHEMA_FIELD(uint16_t, m_unMusicID)
        SCHEMA_FIELD_POINTER(int, m_rank)
    };

    class CPlayer_ItemServices : public CPlayerPawnComponent {
        virtual ~CPlayer_ItemServices() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CPlayer_ItemServices);
    };

    class CCSPlayer_ItemServices : public CPlayer_ItemServices {
        virtual ~CCSPlayer_ItemServices() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CCSPlayer_ItemServices);

        SCHEMA_FIELD(bool, m_bHasDefuser)
        SCHEMA_FIELD(bool, m_bHasHelmet)

    private:
        virtual CBasePlayerWeapon *_GiveNamedItem(const char *pchName) = 0;

    public:
        virtual bool GiveNamedItemBool(const char *pchName) = 0;

        virtual CBasePlayerWeapon *GiveNamedItem(const char *pchName) = 0;

        // Recommended to use CCSPlayer_WeaponServices::DropWeapon instead (parameter is ignored here)
        virtual void DropActiveWeapon(CBasePlayerWeapon *pWeapon) = 0;

        virtual void StripPlayerWeapons(bool removeSuit) = 0;

        // Custom functions
        [[nodiscard]] static bool IsAwsProcessing() noexcept { return g_bAwsChangingTeam; }
        static void ResetAwsProcessing() { g_bAwsChangingTeam = false; }

        [[nodiscard]] static gear_slot_t GetItemGearSlot(const char *item) noexcept;

        CBasePlayerWeapon *GiveNamedItemAws(const char *item) noexcept;

        void RemoveWeapons() {
            CALL_VIRTUAL(void, shared::g_pGameConfig->GetOffset("CCSPlayer_ItemServices_RemoveWeapons"), this);
        }
    };

    // We need an exactly sized class to be able to iterate the vector, our schema system implementation can't do this
    class WeaponPurchaseCount_t {
    private:
        virtual void unk00() {
        };

        virtual void unk01() {
        };

        virtual void unk02() {
        };

        virtual void unk03() {
        };

        virtual void unk04() {
        };

        CCSPlayerPawn *m_pPawn;
        uint64_t unk2 = 0; // 0x10
        uint64_t unk3 = 0; // 0x18
        uint64_t unk4 = 0; // 0x20
        uint64_t unk5 = -1; // 0x28

    public:
        WeaponPurchaseCount_t(CCSPlayerPawn *pPawn, uint16 nItemDefIndex, uint16 nCount) : m_pPawn(pPawn),
            m_nItemDefIndex(nItemDefIndex), m_nCount(nCount) {
            // Since we're constructing a new object, the vtable pointer will be incorrect so fix it
            static const auto pVTable = DynLibUtils::CModule(shared::g_pEntitySystem).GetVirtualTableByName("WeaponPurchaseCount_t");
            ((void **) this)[0] = pVTable;
        }

        uint16_t m_nItemDefIndex; // 0x30
        uint16_t m_nCount; // 0x32
    private:
        uint32_t unk6 = 0;
    };

    struct WeaponPurchaseTracker_t {
    public:
        DECLARE_SCHEMA_CLASS_INLINE(WeaponPurchaseTracker_t)

        SCHEMA_FIELD_POINTER(CUtlVector<WeaponPurchaseCount_t>, m_weaponPurchases)
    };

    class CCSPlayer_ActionTrackingServices : CPlayerPawnComponent {
        virtual ~CCSPlayer_ActionTrackingServices() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CCSPlayer_ActionTrackingServices)

        SCHEMA_FIELD(WeaponPurchaseTracker_t, m_weaponPurchasesThisRound)
    };

    class CPlayer_ObserverServices : public CPlayerPawnComponent {
        virtual ~CPlayer_ObserverServices() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CPlayer_ObserverServices)

        SCHEMA_FIELD(ObserverMode_t, m_iObserverMode)

        SCHEMA_FIELD(CHandle<CBaseEntity>, m_hObserverTarget)

        SCHEMA_FIELD(ObserverMode_t, m_iObserverLastMode)

        SCHEMA_FIELD(bool, m_bForcedObserverMode)
    };

    class CPlayer_CameraServices : public CPlayerPawnComponent {
        virtual ~CPlayer_CameraServices() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CPlayer_CameraServices)

        SCHEMA_FIELD(CHandle<CBaseEntity>, m_hViewEntity)
    };

    class CCSPlayerBase_CameraServices : public CPlayer_CameraServices {
    public:
        virtual ~CCSPlayerBase_CameraServices() = 0;

        DECLARE_SCHEMA_CLASS(CCSPlayerBase_CameraServices)

        SCHEMA_FIELD(CHandle<CBaseEntity>, m_hZoomOwner)

        SCHEMA_FIELD(uint, m_iFOV)
    };

    class CCSPlayer_CameraServices : public CCSPlayerBase_CameraServices {
        virtual ~CCSPlayer_CameraServices() = 0;
    };

    class CCSPlayer_PingServices : public CPlayerPawnComponent {
        virtual ~CCSPlayer_PingServices() = 0;

    public:
        DECLARE_SCHEMA_CLASS(CCSPlayer_PingServices);

        SCHEMA_FIELD_POINTER(GameTime_t, m_flPlayerPingTokens)

        SCHEMA_FIELD(CHandle<CBaseEntity>, m_hPlayerPing)
    };
}
