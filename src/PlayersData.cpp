//
// Created by Michal Přikryl on 12.07.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "PlayersData.h"

namespace TemplatePlugin {
    bool PlayerDataHandler::Add(uint64_t steamid, const PlayerData &data) {
        if (steamid == 0 || players_.contains(steamid)) return false;
        players_[steamid] = data;
        return true;
    }

    PlayerData* PlayerDataHandler::TryGet(uint64_t steamid) {
        auto it = players_.find(steamid);
        return it != players_.end() ? &it->second : nullptr;
    }

    bool PlayerDataHandler::Remove(uint64_t steamid) {
        return players_.erase(steamid) > 0;
    }

    PlayerData& PlayerDataHandler::Ensure(uint64_t steamid) {
        return players_[steamid];
    }

    bool PlayerDataHandler::Add(CCSPlayerController* player, const PlayerData& data) {
        if (!player) return false;
        uint64_t steamid = player->GetSteamID();
        if (steamid == 0) return false;
        return Add(steamid, data);
    }

    PlayerData* PlayerDataHandler::TryGet(CCSPlayerController* player) {
        if (!player) return nullptr;
        uint64_t steamid = player->GetSteamID();
        return steamid ? TryGet(steamid) : nullptr;
    }

    bool PlayerDataHandler::Remove(CCSPlayerController* player) {
        if (!player) return false;
        uint64_t steamid = player->GetSteamID();
        return steamid ? Remove(steamid) : false;
    }

    PlayerData& PlayerDataHandler::Ensure(CCSPlayerController* player) {
        static PlayerData dummy{};
        if (!player) return dummy;
        uint64_t steamid = player->GetSteamID();
        return steamid ? Ensure(steamid) : dummy;
    }

    void PlayerDataHandler::ClearAll() {
        players_.clear();
    }
}
