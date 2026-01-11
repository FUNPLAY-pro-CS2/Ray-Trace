//
// Created by Michal Přikryl on 23.09.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "ctakedamageinfo.h"
#include "dynlibutils/module.h"
#include "CCSPlayerController.h"
#include <Shared.h>

namespace
TemplatePlugin
{
    CAttackerInfo::CAttackerInfo(CEntityInstance* attacker)
    {
        NeedInit = false;
        IsWorld = true;
        IsPawn = false;
        Attacker = attacker ? attacker->m_pEntity->GetRefEHandle().ToInt() : 0;

        if (!attacker || (attacker->m_pEntity && attacker->m_pEntity->m_designerName.String() !=
            "cs_player_controller"))
            return;

        if (auto* controller = static_cast<CCSPlayerController*>(attacker))
        {
            IsWorld = false;
            IsPawn = true;
            AttackerUserId = static_cast<unsigned short>(shared::g_pEngine->GetPlayerUserId(controller->GetPlayerSlot()).Get());
            TeamNum = controller->m_iTeamNum();
            TeamChecked = controller->m_iTeamNum();
        }
    }

    HitGroup_t CTakeDamageInfo::GetHitGroup() const
    {
        const int off = shared::g_pGameConfig->GetOffset("CTakeDamageInfo_HitGroup");
        if (off <= 0)
            return HitGroup_t::HITGROUP_INVALID;

        const uintptr_t base = reinterpret_cast<uintptr_t>(this);

        const uintptr_t v4 = *reinterpret_cast<const uintptr_t*>(base + off);
        if (!v4)
            return HitGroup_t::HITGROUP_INVALID;

        const uintptr_t v1 = *reinterpret_cast<const uintptr_t*>(v4 + 16);
        if (!v1)
            return HitGroup_t::HITGROUP_GENERIC;

        const int32_t group = *reinterpret_cast<const int32_t*>(v1 + 56);
        return static_cast<HitGroup_t>(group);
    }
}
