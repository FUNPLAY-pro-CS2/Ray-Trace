//
// Created by Michal Přikryl on 23.09.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <public/mathlib/vector.h>
#include "ehandle.h"
#include "schemasystem.h"
#include "globaltypes.h"

namespace
TemplatePlugin
{
	class CBaseEntity;
    class CTakeDamageInfo;

#pragma pack(push, 1)
    struct CAttackerInfo
    {
        bool NeedInit = true;             // 0x0
        bool IsPawn = false;              // 0x1
        bool IsWorld = false;             // 0x2
        std::uint8_t pad_3[1] = {};       // vyrovnání na 4 bajty
        std::uint32_t Attacker = 0;       // 0x4
        std::uint16_t AttackerUserId = 0; // 0x8
        std::uint8_t pad_A[2] = {};       // vyrovnání na 0xC
        int TeamChecked = -1;             // 0x0C
        int TeamNum = -1;                 // 0x10

        CAttackerInfo() = default;
        explicit CAttackerInfo(CEntityInstance* attacker);
    };
#pragma pack(pop)

    struct CTakeDamageInfoContainer
    {
        CTakeDamageInfo* pInfo;
    };

    class CTakeDamageInfo
    {
    public:
        DECLARE_SCHEMA_CLASS(CTakeDamageInfo)

    	SCHEMA_FIELD(Vector, m_vecDamageForce);
    	SCHEMA_FIELD(Vector, m_vecDamagePosition);
    	SCHEMA_FIELD(Vector, m_vecReportedPosition);
		SCHEMA_FIELD(Vector, m_vecDamageDirection);
    	SCHEMA_FIELD(CHandle<CBaseEntity>, m_hInflictor);
    	SCHEMA_FIELD(CHandle<CBaseEntity>, m_hAttacker);
    	SCHEMA_FIELD(CHandle<CBaseEntity>, m_hAbility);
    	SCHEMA_FIELD(float, m_flDamage);
    	SCHEMA_FIELD(float, m_flTotalledDamage);
    	SCHEMA_FIELD(int32_t, m_bitsDamageType);
    	SCHEMA_FIELD(int32_t, m_iDamageCustom);
    	SCHEMA_FIELD(int8_t, m_iAmmoType);
    	SCHEMA_FIELD(float, m_flOriginalDamage);
    	SCHEMA_FIELD(bool, m_bShouldBleed);
    	SCHEMA_FIELD(bool, m_bShouldSpark);
		SCHEMA_FIELD(TakeDamageFlags_t, m_nDamageFlags);
		SCHEMA_FIELD_POINTER(char, m_sDamageSourceName);
        SCHEMA_FIELD(HitGroup_t, m_iHitGroupId);
        SCHEMA_FIELD(int32_t, m_nNumObjectsPenetrated);
        SCHEMA_FIELD(float, m_flFriendlyFireDamageReductionRatio);
        SCHEMA_FIELD(bool, m_bInTakeDamageFlow);

        HitGroup_t GetHitGroup() const;
    };

	class CTakeDamageResult
	{
	public:
		DECLARE_SCHEMA_CLASS(CTakeDamageResult)

		SCHEMA_FIELD(CTakeDamageInfo*, m_pOriginatingInfo)
		SCHEMA_FIELD(int32, m_nHealthLost)
		SCHEMA_FIELD(int32, m_nHealthBefore)
		SCHEMA_FIELD(int32, m_nDamageDealt)
		SCHEMA_FIELD(float32, m_flPreModifiedDamage)
		SCHEMA_FIELD(int32, m_nTotalledHealthLost)
		SCHEMA_FIELD(int32, m_nTotalledDamageDealt)
		SCHEMA_FIELD(bool, m_bWasDamageSuppressed)
	};
}
