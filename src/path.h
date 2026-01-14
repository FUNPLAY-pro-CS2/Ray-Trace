//
// Created by Michal Přikryl on 19.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include <eiface.h>
#include <string>
#include "Shared.h"

namespace RayTracePlugin::Paths {
    static std::string gameDirectory;

    inline std::string GameDirectory() {
        if (gameDirectory.empty()) {
            CBufferStringGrowable<255> gamePath;
            shared::g_pEngine->GetGameDir(gamePath);
            gameDirectory = std::string(gamePath.Get());
        }
        return gameDirectory;
    }

    inline std::string GetRootDirectory() { return GameDirectory() + "/addons/RayTrace"; }
    inline std::string EnginePath() { return GameDirectory() + "../bin/linuxsteamrt64/libengine2.so"; }
    inline std::string Tier0Path() { return GameDirectory() + "../bin/linuxsteamrt64/libtier0.so"; }
    inline std::string ServerPath() { return GameDirectory() + "/bin/linuxsteamrt64/libserver.so"; }
    inline std::string SchemaSystemPath() { return GameDirectory() + "../bin/linuxsteamrt64/libschemasystem.so"; }
    inline std::string VScriptPath() { return GameDirectory() + "../bin/linuxsteamrt64/libvscript.so"; }
} // namespace RayTracePlugin::Paths