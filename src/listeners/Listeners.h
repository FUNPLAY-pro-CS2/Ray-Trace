//
// Created by Michal Přikryl on 09.07.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <eiface.h>
#include <sourcehook.h>
#include <iserver.h>

namespace RayTracePlugin::Listeners {
    void InitListeners();

    void DestructListeners();

    class SourceHooks {
    public:
        void Hook_GameFrame(bool simulating, bool bFirstTick, bool bLastTick);
        void Hook_StartupServer(const GameSessionConfiguration_t& config, ISource2WorldSession*, const char*);
        int Hook_LoadEventsFromFile(const char* filename, bool bSearchAll);
    };

    extern SourceHooks sourceHooks;
}
