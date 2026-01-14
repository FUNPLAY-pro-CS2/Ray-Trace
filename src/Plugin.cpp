#include "Plugin.h"
#include "path.h"
#include "Shared.h"
#include "dynlibutils/module.h"
#include <entitysystem.h>
#include "igameevents.h"
#include <iserver.h>
#include "schemasystem/schemasystem.h"
#include "schema/cgameresourceserviceserver.h"
#include "schema/plat.h"
#include <filesystem>
#include <cstdio>
#include <fstream>
#include <gameconfig.h>
#include <regex>
#include <listeners/Listeners.h>
#include "CRayTraceInterface.h"
#include "log.h"
#include "RayTrace.h"
#include "tasks.h"

#define VERSION_STRING SEMVER " @ " GITHUB_SHA
#define BUILD_TIMESTAMP __DATE__ " " __TIME__

PLUGIN_EXPOSE(RayTrace, RayTracePlugin::g_iPlugin);

CGameEntitySystem* GameEntitySystem()
{
    return *reinterpret_cast<CGameEntitySystem**>((uintptr_t)(g_pGameResourceServiceServer) +
        RayTracePlugin::shared::g_pGameConfig->GetOffset("GameEntitySystem"));
}

class GameSessionConfiguration_t
{
};

namespace RayTracePlugin
{
    IPlugin g_iPlugin;

    bool IPlugin::Load(PluginId id, ISmmAPI* ismm, char* error, size_t maxlen, bool late)
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

        Log::Init();
        Tasks::Init();

        auto gamedata_path = std::string(Paths::GetRootDirectory() + "/gamedata.json");
        shared::g_pGameConfig = new CGameConfig(gamedata_path);
        char conf_error[255] = "";

        if (!shared::g_pGameConfig->Init(conf_error, sizeof(conf_error)))
        {
            FP_ERROR("Could not read '{}'. Error: {}", gamedata_path, conf_error);
            return false;
        }

        g_SMAPI->AddListener(this, this);
        Listeners::InitListeners();

        g_pCVar = shared::g_pCVar;
        ConVar_Register(FCVAR_RELEASE | FCVAR_CLIENT_CAN_EXECUTE | FCVAR_GAMEDLL);

        if (late)
        {
            shared::g_pEntitySystem = GameEntitySystem();
            RayTrace::Initialize();
            shared::g_bDetoursLoaded = true;
        }

        FP_INFO("<<< Load() success! >>>");
        return true;
    }

    bool IPlugin::Unload(char* error, size_t maxlen)
    {
        Listeners::DestructListeners();
        Tasks::Shutdown();

        FP_INFO("<<< Unload() success! >>>");

        return true;
    }

    void *IPlugin::OnMetamodQuery(const char *iface, int *ret) {
        if (strcmp(iface, RAYTRACE_INTERFACE_VERSION) == 0) {
            *ret = META_IFACE_OK;
            FP_INFO("{} accessed.", RAYTRACE_INTERFACE_VERSION);
            return &RayTrace::g_CRayTrace;
        }

        *ret = META_IFACE_FAILED;
        return nullptr;
    }

    const char* IPlugin::GetAuthor() { return "Slynx"; }
    const char* IPlugin::GetName() { return "RayTrace"; }
    const char* IPlugin::GetDescription() { return "RayTrace Metamod plugin for CS2 servers."; }
    const char* IPlugin::GetURL() { return "https://slynxdev.cz"; }
    const char* IPlugin::GetLicense() { return "GPLv3"; }
    const char* IPlugin::GetVersion() { return VERSION_STRING; }
    const char* IPlugin::GetDate() { return BUILD_TIMESTAMP; }
    const char* IPlugin::GetLogTag() { return "RayTrace"; }
}
