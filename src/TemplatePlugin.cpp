#include "TemplatePlugin.h"
#include "path.h"
#include "Shared.h"
#include "dynlibutils/module.h"
#include <entitysystem.h>
#include "igameevents.h"
#include <iserver.h>
#include "game_system.h"
#include "schemasystem/schemasystem.h"
#include "schema/CCSPlayerController.h"
#include "schema/cgameresourceserviceserver.h"
#include "schema/plat.h"
#include <filesystem>
#include <cstdio>
#include <detours.h>
#include <fstream>
#include <gameconfig.h>
#include <regex>
#include <listeners/Listeners.h>
#include "EntityData.h"
#include "log.h"
#include "PlayersData.h"
#include "tasks.h"
#include "commands/Commands.h"
#include "events/Events.h"
#include "hooks/Hooks.h"
#include "schema/CGameRules.h"

#define VERSION_STRING SEMVER " @ " GITHUB_SHA
#define BUILD_TIMESTAMP __DATE__ " " __TIME__

PLUGIN_EXPOSE(Template, TemplatePlugin::g_iPlugin);

CGameEntitySystem* GameEntitySystem()
{
    return *reinterpret_cast<CGameEntitySystem**>((uintptr_t)(g_pGameResourceServiceServer) +
        TemplatePlugin::shared::g_pGameConfig->GetOffset("GameEntitySystem"));
}

class GameSessionConfiguration_t
{
};

namespace TemplatePlugin
{
    ITemplatePlugin g_iPlugin;

    bool ITemplatePlugin::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
    {
        PLUGIN_SAVEVARS();

        GET_V_IFACE_CURRENT(GetEngineFactory, shared::g_pCVar, ICvar, CVAR_INTERFACE_VERSION);
        GET_V_IFACE_ANY(GetServerFactory, shared::g_pSource2Server, ISource2Server, SOURCE2SERVER_INTERFACE_VERSION);
        GET_V_IFACE_ANY(GetServerFactory, shared::g_pServer, IServerGameDLL, INTERFACEVERSION_SERVERGAMEDLL);
        GET_V_IFACE_CURRENT(GetEngineFactory, shared::g_pEngine, IVEngineServer, INTERFACEVERSION_VENGINESERVER);
        GET_V_IFACE_ANY(GetEngineFactory, shared::g_pSchemaSystem, CSchemaSystem, SCHEMASYSTEM_INTERFACE_VERSION);
        GET_V_IFACE_ANY(GetEngineFactory, shared::g_pGameEventSystem, IGameEventSystem,
                        GAMEEVENTSYSTEM_INTERFACE_VERSION);
        GET_V_IFACE_ANY(GetServerFactory, shared::g_pGameEntities, ISource2GameEntities,
                        SOURCE2GAMEENTITIES_INTERFACE_VERSION);
        GET_V_IFACE_ANY(GetServerFactory, shared::g_pGameClients, IServerGameClients,
                        SOURCE2GAMECLIENTS_INTERFACE_VERSION);
        GET_V_IFACE_CURRENT(GetEngineFactory, g_pGameResourceServiceServer, IGameResourceService,
                            GAMERESOURCESERVICESERVER_INTERFACE_VERSION);
        GET_V_IFACE_ANY(GetEngineFactory, shared::g_pNetworkMessages, INetworkMessages,
                        NETWORKMESSAGES_INTERFACE_VERSION);
        GET_V_IFACE_ANY(GetEngineFactory, shared::g_pNetworkServerService, INetworkServerService,
                        NETWORKSERVERSERVICE_INTERFACE_VERSION);

        g_pCVar = shared::g_pCVar;
        g_pSource2GameEntities = shared::g_pGameEntities;
        shared::g_pGameResourceServiceServer = (CGameResourceService*)g_pGameResourceServiceServer;
        if (!shared::g_pGameResourceServiceServer)
            return false;

        Tasks::Init();
        auto gamedata_path = std::string(Paths::GetRootDirectory() + "/gamedata.json");
        shared::g_pGameConfig = new CGameConfig(gamedata_path);
        char conf_error[255] = "";

        if (!shared::g_pGameConfig->Init(conf_error, sizeof(conf_error)))
        {
            FP_ERROR("Could not read '{}'. Error: {}", gamedata_path, conf_error);
            return false;
        }

        if (!InitGameSystems())
            return false;

        g_SMAPI->AddListener(this, this);
        Listeners::InitListeners();

        g_pCVar = shared::g_pCVar;
        ConVar_Register(FCVAR_RELEASE | FCVAR_CLIENT_CAN_EXECUTE | FCVAR_GAMEDLL);

        if (late)
        {
            shared::g_pEntitySystem = GameEntitySystem();
            shared::g_pEntitySystem->AddListenerEntity(&Detours::entityListener);
            Commands::InitCommands();
            Events::InitEvents();
            Hooks::InitHooks();
            Detours::InitHooks();
            RayTrace::Initialize();
            shared::g_bDetoursLoaded = true;
        }

        FP_INFO("<<< Load() success! >>>");
        return true;
    }

    bool ITemplatePlugin::Unload(char* error, size_t maxlen)
    {
        Listeners::DestructListeners();
        Detours::ShutdownHooks();
        Detours::Shutdown();
        shared::g_pEntitySystem->RemoveListenerEntity(&Detours::entityListener);
        Tasks::Shutdown();

        FP_INFO("<<< Unload() success! >>>");

        return true;
    }

    const char* ITemplatePlugin::GetAuthor() { return "Slynx"; }
    const char* ITemplatePlugin::GetName() { return "TemplatePlugin"; }
    const char* ITemplatePlugin::GetDescription() { return "TemplatePlugin Metamod plugin for CS2 servers."; }
    const char* ITemplatePlugin::GetURL() { return "https://slynxdev.cz"; }
    const char* ITemplatePlugin::GetLicense() { return "GPLv3"; }
    const char* ITemplatePlugin::GetVersion() { return VERSION_STRING; }
    const char* ITemplatePlugin::GetDate() { return BUILD_TIMESTAMP; }
    const char* ITemplatePlugin::GetLogTag() { return "TemplatePlugin"; }
}
