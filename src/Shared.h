//
// Created by Michal Přikryl on 20.09.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <icvar.h>
#include <memory>
#include <schemasystem.h>
#include <vector>
#include <eiface.h>
#include <gameconfig.h>
#include <igameeventsystem.h>
#include "schema/cgameresourceserviceserver.h"
#include <sourcehook/sourcehook.h>

class CGameEntitySystem;

namespace TemplatePlugin::shared
{
    extern ICvar* g_pCVar;
    extern IServerGameDLL* g_pServer;
    extern ISource2Server* g_pSource2Server;
    extern IVEngineServer* g_pEngine;
    extern CSchemaSystem* g_pSchemaSystem;
    extern IGameEventManager2* g_pGameEventManager;
    extern IGameEventSystem* g_pGameEventSystem;
    extern ISource2GameEntities* g_pGameEntities;
    extern INetworkMessages* g_pNetworkMessages;
    extern INetworkServerService* g_pNetworkServerService;
    extern CGameEntitySystem* g_pEntitySystem;
    extern IServerGameClients* g_pGameClients;
    extern CGlobalVars* g_pGlobalVars;
    extern CGameResourceService* g_pGameResourceServiceServer;
    extern CGameConfig *g_pGameConfig;

    extern SourceHook::ISourceHook *source_hook;
    extern int source_hook_pluginid;

    CGlobalVars* getGlobalVars();
    extern const char *GetMapName();
    extern void ServerCommand(const char *command);
    extern double GetEngineTime();
    extern float GetTickInterval();
    extern float GetCurrentTime();
    extern int GetTickCount();
    extern float GetGameFrameTime();

    extern bool g_bHasTicked;
    extern bool g_bDetoursLoaded;
}

#undef SH_GLOB_SHPTR
#define SH_GLOB_SHPTR TemplatePlugin::shared::source_hook
#undef SH_GLOB_PLUGPTR
#define SH_GLOB_PLUGPTR TemplatePlugin::shared::source_hook_pluginid
