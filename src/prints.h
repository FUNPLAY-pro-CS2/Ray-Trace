//
// Created by Michal Přikryl on 10.07.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <string>
#include <vector>
#include "schema/CCSPlayerController.h"
#include "log.h"

namespace TemplatePlugin::Prints
{
    enum class HudDestination : int
     {
         Center = 1,
         Alert = 2,
         Chat = 3
     };

    void ClientPrintNative(CPlayerSlot slot, HudDestination dest, const char* msg);

    std::string ReplaceColorTags(const std::string& input);

    std::string ReplaceFormatTags(const std::string &input);

    template<typename... Args>
    std::string FormatMessage(fmt::format_string<Args...> fmtStr, Args &&... args) {
        std::string formatted = fmt::format(fmtStr, std::forward<Args>(args)...);
        return ReplaceFormatTags(formatted);
    }

    inline const char* PrefixCl() { return "[[DARKRED]][!][[DEFAULT]] "; }

    void ChatToPlayer(CCSPlayerController* player, const std::string& msg);
}
