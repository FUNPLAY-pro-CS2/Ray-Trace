//
// Created by Michal Přikryl on 09.07.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include <funchook/include/funchook.h>
#include <igameevents.h>
#include <sourcehook.h>
#include <cstring>
#include <unordered_set>
#include "schema/CCSPlayerController.h"
#include "detours.h"

#include <Shared.h>
#include <TemplatePlugin.h>

#include "log.h"

namespace TemplatePlugin {
    SH_DECL_HOOK2(IGameEventManager2, FireEvent, SH_NOATTRIB, 0, bool, IGameEvent*, bool);
    SH_DECL_HOOK3_void(ICvar, DispatchConCommand, SH_NOATTRIB, 0, ConCommandRef, const CCommandContext&,
                       const CCommand&);

    namespace Detours {
        std::vector<std::unique_ptr<ConCommand> > registeredCommands;
        static std::unordered_map<std::string, std::vector<CommandEntry> > consoleListeners;
        static std::unordered_map<std::string, std::vector<EventEntry> > gameEvents;
        static EventManager eventManager;
        static std::vector<IGameEvent *> eventStack;
        static std::vector<EntityEventHandler> entitySpawnedListeners;
        static std::vector<EntityEventHandler> entityCreatedListeners;
        static std::vector<EntityEventHandler> entityDeletedListeners;
        static std::vector<EntityParentChangedHandler> entityParentChangerListeners;
        static std::unordered_set<std::string> registeredNames;
        static std::unordered_map<std::string, CommandHandler> commandCallbacks;
        struct OutputHookKey {
            CEntityInstance *entity;
            std::string output;
            HookMode mode;

            bool operator==(const OutputHookKey &o) const noexcept {
                return entity == o.entity && output == o.output && mode == o.mode;
            }
        };

        struct OutputHookKeyHasher {
            size_t operator()(const OutputHookKey &k) const noexcept {
                return std::hash<void *>{}(k.entity) ^ (std::hash<std::string>{}(k.output) << 1)
                       ^ (std::hash<int>{}(static_cast<int>(k.mode)) << 2);
            }
        };
        static std::unordered_map<OutputHookKey, std::vector<EntityOutputHandler>, OutputHookKeyHasher>
                g_entityOutputHooks;

        void ConCommandRouter(const CCommandContext &ctx, const CCommand &args) {
            if (args.ArgC() < 1)
                return;

            std::string name = args.Arg(0);
            auto it = commandCallbacks.find(name);
            if (it == commandCallbacks.end())
                return;

            (void) it->second(ctx, args, HookMode::Post);
        }

        void RegisterChatListener(const std::string &name,
                                  const std::function<void(const CCommandContext &, const CCommand &, HookMode)> &
                                  handler) {
            CommandHandler nativeHandler = WrapVoidHandler(handler);

            RegisterConsoleListener(name, nativeHandler, HookMode::Pre);
            RegisterConsoleListener("/" + name, nativeHandler, HookMode::Pre);
            RegisterConsoleListener("!" + name, nativeHandler, HookMode::Pre);
        }

        void RegisterConsoleCommand(const std::string &name,
                                    const std::function<void(const CCommandContext &, const CCommand &, HookMode)> &
                                    handler) {
            CommandHandler nativeHandler = WrapVoidHandler(handler);

            if (shared::g_pCVar && shared::g_pCVar->FindConCommand(name.c_str()).IsValidRef()) {
                RegisterConsoleListener(name, nativeHandler, HookMode::Pre);
                RegisterConsoleListener("/" + name, nativeHandler, HookMode::Pre);
                RegisterConsoleListener("!" + name, nativeHandler, HookMode::Pre);
                return;
            }

            if (!registeredNames.contains(name)) {
                auto cmd = std::make_unique<ConCommand>(
                    name.c_str(),
                    ConCommandRouter,
                    ("Registered command: " + name).c_str(),
                    FCVAR_NONE
                );
                registeredCommands.push_back(std::move(cmd));
                registeredNames.insert(name);
            }

            RegisterConsoleListener(name, nativeHandler, HookMode::Pre);
            RegisterConsoleListener("/" + name, nativeHandler, HookMode::Pre);
            RegisterConsoleListener("!" + name, nativeHandler, HookMode::Pre);
        }

        void InitHooks() {
            SH_ADD_HOOK(IGameEventManager2, FireEvent, shared::g_pGameEventManager, Detours::OnFireEvent, false);
            SH_ADD_HOOK(IGameEventManager2, FireEvent, shared::g_pGameEventManager, Detours::OnFireEventPost, true);
            SH_ADD_HOOK(ICvar, DispatchConCommand, shared::g_pCVar, Hook_DispatchConCommand, false);
        }

        void ShutdownHooks() {
            SH_REMOVE_HOOK(IGameEventManager2, FireEvent, shared::g_pGameEventManager, Detours::OnFireEvent, false);
            SH_REMOVE_HOOK(IGameEventManager2, FireEvent, shared::g_pGameEventManager, Detours::OnFireEventPost, true);
            SH_REMOVE_HOOK(ICvar, DispatchConCommand, shared::g_pCVar, Hook_DispatchConCommand, false);
        }

        void RegisterConsoleListener(const std::string &name, CommandHandler handler, HookMode mode) {
            consoleListeners[name].push_back({handler, mode});
        }

        void RegisterGameEvent(const std::string &name, GameEventHandler handler, HookMode mode) {
            gameEvents[name].push_back({handler, mode});
            if (!shared::g_pGameEventManager->FindListener(&eventManager, name.c_str()))
            {
                shared::g_pGameEventManager->AddListener(&eventManager, name.c_str(), true);
            }
        }

        void EventManager::FireGameEvent(IGameEvent* pEvent) {}

        void RegisterEntityListener(EntityEventHandler handler, EntityEventType type) {
            switch (type) {
                case EntityEventType::ENTITY_SPAWNED:
                    entitySpawnedListeners.push_back(handler);
                    break;
                case EntityEventType::ENTITY_CREATED:
                    entityCreatedListeners.push_back(handler);
                    break;
                case EntityEventType::ENTITY_DELETED:
                    entityDeletedListeners.push_back(handler);
                    break;
            }
        }

        void *FindModuleSignature(const DynLibUtils::CModule& module, const char *name) {
            return module.FindPattern(
                shared::g_pGameConfig->GetSignature(name)
            );
        }

        HookResult DispatchConsoleListener(const CCommandContext &ctx, const CCommand &args, HookMode mode) {
            std::string name = args.Arg(0);
            std::transform(name.begin(), name.end(), name.begin(),
                           [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

            auto it = consoleListeners.find(name);
            if (it == consoleListeners.end())
                return HookResult::Continue;

            HookResult result = HookResult::Continue;

            for (const auto &entry: it->second) {
                if (entry.mode != mode)
                    continue;

                HookResult thisResult = entry.handler(ctx, args, mode);

                if (thisResult == HookResult::Stop)
                    return HookResult::Stop;

                if (thisResult == HookResult::Handled && mode == HookMode::Pre)
                    return HookResult::Handled;

                if (static_cast<int>(thisResult) > static_cast<int>(result))
                    result = thisResult;
            }

            return result;
        }

        bool DispatchGameEvent(IGameEvent *event, HookMode mode, bool &dontBroadcast) {
            const char *name = event->GetName();
            auto it = gameEvents.find(name);
            if (it == gameEvents.end())
                return true;

            EventOverride override{dontBroadcast};

            for (const auto &hook: it->second) {
                if (hook.mode != mode)
                    continue;

                HookResult result = hook.handler(event, mode, override);

                if (result == HookResult::Handled || result == HookResult::Stop)
                    return false;
            }

            dontBroadcast = override.dontBroadcast;

            return true;
        }

        void DispatchEntitySpawn(CEntityInstance *ent) {
            for (auto &fn: entitySpawnedListeners) {
                fn(ent);
            }
        }

        void DispatchEntityCreate(CEntityInstance *ent) {
            for (auto &fn: entityCreatedListeners) {
                fn(ent);
            }
        }

        void DispatchEntityDelete(CEntityInstance *ent) {
            for (auto &fn: entityDeletedListeners) {
                fn(ent);
            }
        }

        void HookSingleEntityOutput(CEntityInstance *entity,
                                    const std::string &outputName,
                                    EntityOutputHandler handler,
                                    HookMode mode) {
            g_entityOutputHooks[{entity, outputName, mode}].push_back(std::move(handler));
        }

        void UnhookSingleEntityOutput(CEntityInstance *entity,
                                      const std::string &outputName,
                                      EntityOutputHandler handler,
                                      HookMode mode) {
            auto key = OutputHookKey{entity, outputName, mode};
            auto it = g_entityOutputHooks.find(key);
            if (it == g_entityOutputHooks.end()) return;

            auto &vec = it->second;
            vec.erase(std::remove_if(vec.begin(), vec.end(),
                                     [&](const EntityOutputHandler &h) {
                                         return h.target<void>() == handler.target<void>();
                                     }),
                      vec.end());

            if (vec.empty())
                g_entityOutputHooks.erase(it);
        }

        HookResult DispatchEntityOutput(CEntityIOOutput *output,
                                        const char *name,
                                        CEntityInstance *activator,
                                        CEntityInstance *caller,
                                        const CVariant *value,
                                        float delay,
                                        HookMode mode) {
            HookResult result = HookResult::Continue;

            for (auto &[key, handlers]: g_entityOutputHooks) {
                if (key.entity != caller) continue;
                if (key.output != name) continue;
                if (key.mode != mode) continue;

                for (auto &fn: handlers) {
                    HookResult r = fn(output, name, activator, caller, value, delay);
                    if (r == HookResult::Stop) return HookResult::Stop;
                    if (static_cast<int>(r) > static_cast<int>(result))
                        result = r;
                }
            }
            return result;
        }

        bool OnFireEvent(IGameEvent *event, bool bDontBroadcast) {
            if (!event)
                RETURN_META_VALUE(MRES_IGNORED, false);

            bool localDontBroadcast = bDontBroadcast;
            if (!DispatchGameEvent(event, HookMode::Pre, localDontBroadcast))
                RETURN_META_VALUE(MRES_SUPERCEDE, false);

            if (IGameEvent *copy = shared::g_pGameEventManager->DuplicateEvent(event)) eventStack.push_back(copy);

            if (localDontBroadcast != bDontBroadcast)
                RETURN_META_VALUE_NEWPARAMS(MRES_IGNORED, true, &IGameEventManager2::FireEvent,
                                        (event, localDontBroadcast));

            RETURN_META_VALUE(MRES_IGNORED, true);
        }

        bool OnFireEventPost(IGameEvent *event, bool bDontBroadcast) {
            if (!event)
                RETURN_META_VALUE(MRES_IGNORED, false);

            if (!eventStack.empty()) {
                IGameEvent *copy = eventStack.back();
                eventStack.pop_back();

                bool dummy = bDontBroadcast;
                DispatchGameEvent(copy, HookMode::Post, dummy);
                shared::g_pGameEventManager->FreeEvent(copy);
            }

            RETURN_META_VALUE(MRES_IGNORED, true);
        }

        void Hook_DispatchConCommand(ConCommandRef, const CCommandContext &ctx, const CCommand &args) {
            if (args.ArgC() >= 2) {
                const char *cmd = args.Arg(0);
                const char *msg = args.Arg(1);

                if (V_strcmp(cmd, "say") == 0 || V_strcmp(cmd, "say_team") == 0) {
                    std::string message = msg;

                    if (message.size() >= 2 && message.front() == '"' && message.back() == '"')
                        message = message.substr(1, message.size() - 2);

                    if (!message.empty() && (message[0] == '!' || message[0] == '/')) {
                        std::string cleaned = message.substr(1);

                        CCommand parsed;
                        parsed.Tokenize(cleaned.c_str());

                        if (parsed.ArgC() > 0) {
                            HookResult r = DispatchConsoleListener(ctx, parsed, HookMode::Pre);
                            if (r != HookResult::Stop)
                                DispatchConsoleListener(ctx, parsed, HookMode::Post);
                        }

                        RETURN_META(MRES_SUPERCEDE);
                    }
                }
            }

            HookResult result = DispatchConsoleListener(ctx, args, HookMode::Pre);

            if (result == HookResult::Handled || result == HookResult::Stop)
                RETURN_META(MRES_SUPERCEDE);

            DispatchConsoleListener(ctx, args, HookMode::Post);
        }

        void Shutdown() {
            consoleListeners.clear();
            shared::g_pGameEventManager->RemoveListener(&eventManager);
            gameEvents.clear();
            entitySpawnedListeners.clear();
            entityCreatedListeners.clear();
            entityDeletedListeners.clear();
            entityParentChangerListeners.clear();
            commandCallbacks.clear();
            registeredNames.clear();
            registeredCommands.clear();
            for (auto hook: hookHandles) {
                if (!hook) continue;

                int rc = funchook_uninstall(hook, 0);
                if (rc != 0) {
                    FP_ERROR("Failed to uninstall hook: {}", rc);
                }

                rc = funchook_destroy(hook);
                if (rc != 0) {
                    FP_ERROR("Failed to destroy hook: {}", rc);
                }
            }
            hookHandles.clear();
            signatureHooks.clear();
            shared::g_pEntitySystem->RemoveListenerEntity(&entityListener);
        }
    }
}
