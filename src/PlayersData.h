//
// Created by Michal Přikryl on 12.07.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <unordered_map>
#include "schema/CCSPlayerController.h"

namespace TemplatePlugin {
    struct PlayerData {
    };

    class PlayerDataHandler {
    public:
        static bool Add(uint64_t steamid, const PlayerData &data = {});
        static PlayerData* TryGet(uint64_t steamid);
        static bool Remove(uint64_t steamid);
        static PlayerData& Ensure(uint64_t steamid);

        static bool Add(CCSPlayerController* player, const PlayerData& data = {});
        static PlayerData* TryGet(CCSPlayerController* player);
        static bool Remove(CCSPlayerController* player);
        static PlayerData& Ensure(CCSPlayerController* player);

        static void ClearAll();

    private:
        static inline std::unordered_map<uint64_t, PlayerData> players_;
    };
}
