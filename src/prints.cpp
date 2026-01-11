//
// Created by Michal Přikryl on 10.07.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "prints.h"
#include "PlayersData.h"
#include <igameeventsystem.h>
#include <regex>
#include <networksystem/inetworkmessages.h>
#include "usermessages.pb.h"
#include "tier0/memdbgon.h"

namespace TemplatePlugin::Prints
{
    std::string ReplaceColorTags(const std::string &input) {
        static const std::vector<std::pair<std::string, std::string> > tags = {
            {"[[DEFAULT]]", "\x01"}, {"[[DARKRED]]", "\x02"}, {"[[LIGHTPURPLE]]", "\x03"}, {"[[GREEN]]", "\x04"},
            {"[[OLIVE]]", "\x05"}, {"[[LIME]]", "\x06"}, {"[[RED]]", "\x07"}, {"[[GREY]]", "\x08"},
            {"[[YELLOW]]", "\x09"}, {"[[SILVER]]", "\x0A"}, {"[[BLUE]]", "\x0B"}, {"[[DARKBLUE]]", "\x0C"},
            {"[[ORANGE]]", "\x10"}, {"[[PURPLE]]", "\x0E"}
        };

        std::string result = input;
        for (const auto &[tag, code]: tags) {
            size_t pos;
            while ((pos = result.find(tag)) != std::string::npos)
                result.replace(pos, tag.length(), code);
        }

        return result;
    }

    std::string ReplaceFormatTags(const std::string &input) {
        std::string result = ReplaceColorTags(input);

        return result;
    }

    void ChatToPlayer(CCSPlayerController* player, const std::string& msg)
    {
        if (!player || !player->CheckValid())
            return;

        ClientPrintNative(player->GetPlayerSlot(), HudDestination::Chat, ReplaceColorTags(" " + msg).c_str());
    }

    void ClientPrintNative(CPlayerSlot slot, HudDestination dest, const char* msg)
    {
        INetworkMessageInternal* pNetMsg = shared::g_pNetworkMessages->FindNetworkMessagePartial("TextMsg");
        auto data = pNetMsg->AllocateMessage()->ToPB<CUserMessageTextMsg>();
        data->set_dest(static_cast<google::protobuf::uint32>(dest));
        data->add_param(msg);

        CPlayerBitVec recipients;
        recipients.Set(slot.Get());

        shared::g_pGameEventSystem->PostEventAbstract(
            CSplitScreenSlot(-1), false, ABSOLUTE_PLAYER_LIMIT,
            reinterpret_cast<const uint64*>(recipients.Base()), pNetMsg, data,
            0, BUF_RELIABLE
        );

        delete data;
    }
}
