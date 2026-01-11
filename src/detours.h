//
// Created by Michal Přikryl on 20.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include <any>
#include <functional>
#include <string>
#include <igameevents.h>
#include <convar.h>
#include <features.h>
#include <ISmmPlugin.h>
#include <TemplatePlugin.h>
#include <funchook/include/funchook.h>
#include <type_traits>
#include "detourtypes.h"
#include "entitysystem.h"
#include "log.h"
#include "dynlibutils/module.h"

namespace TemplatePlugin::Detours
{
    struct CommandEntry
    {
        CommandHandler handler;
        HookMode mode;
    };

    struct EventEntry
    {
        GameEventHandler handler;
        HookMode mode;
    };

    class EventManager : public IGameEventListener2
    {
        void FireGameEvent(IGameEvent* pEvent) override;
    };

    static std::unordered_map<std::string, std::vector<SignatureEntry>> signatureHooks;
    static std::mutex signatureHooksMutex;
    static std::vector<funchook_t*> hookHandles;
    extern std::unordered_map<std::string, std::any> overriddenReturns;
    extern std::mutex overriddenReturnsMutex;

    void Shutdown();

    void InitHooks();

    void ShutdownHooks();

    bool OnFireEvent(IGameEvent* event, bool bDontBroadcast);

    bool OnFireEventPost(IGameEvent* event, bool bDontBroadcast);

    void ConCommandRouter(const CCommandContext& ctx, const CCommand& args);

    void Hook_DispatchConCommand(ConCommandRef, const CCommandContext&, const CCommand&);

    void RegisterConsoleListener(const std::string& name, CommandHandler handler, HookMode mode = HookMode::Post);

    void RegisterChatListener(const std::string& name,
                              const std::function<void(const CCommandContext&, const CCommand&, HookMode)>& handler);

    void RegisterConsoleCommand(const std::string& name,
                                const std::function<void(const CCommandContext&, const CCommand&,
                                                         HookMode)>& handler);

    void RegisterGameEvent(const std::string& name, GameEventHandler handler, HookMode mode = HookMode::Post);

    void RegisterEntityListener(EntityEventHandler handler, EntityEventType type);

    inline void RegisterSignatureHandler(const std::string& name,
                                         SignatureHandler handler,
                                         HookMode mode = HookMode::Post)
    {
        std::scoped_lock lock(signatureHooksMutex);
        signatureHooks[name].push_back({handler, mode});
    }

    void* FindModuleSignature(const DynLibUtils::CModule& module, const char* name);

    template <typename T>
    inline void* ToVoidArg(T&& v)
    {
        return const_cast<void*>(reinterpret_cast<const void*>(&v));
    }

    template <typename T>
    inline void* ToVoidArg(T* v)
    {
        return const_cast<void*>(reinterpret_cast<const void*>(v));
    }

    template <typename Fn, typename... Args>
    auto DispatchSignatureCall(const std::string& name, Fn original, Args... args)
        -> decltype(original(args...))
    {
        using Ret = decltype(original(args...));
        std::vector<SignatureEntry> hooksCopy;

        {
            std::scoped_lock lock(signatureHooksMutex);
            auto it = signatureHooks.find(name);
            if (it != signatureHooks.end())
                hooksCopy = it->second;
        }

        void* argArray[] = {ToVoidArg(args)...};
        constexpr size_t argCount = sizeof...(Args);
        void* self = argCount > 0 ? argArray[0] : nullptr;

        bool skipOriginal = false;
        bool allowReturnOverride = false;

        if constexpr (!std::is_void_v<Ret>)
        {
            Ret result{};
            DynamicHook hook{argArray, argCount, self, &result};

            // --- PRE hooks ---
            for (auto& entry : hooksCopy)
            {
                if (entry.mode != HookMode::Pre) continue;
                HookResult r = entry.handler(&hook, HookMode::Pre);
                if (r == HookResult::Stop) return result;
                if (r == HookResult::Handled)
                {
                    skipOriginal = true;
                    return result;
                }
                if (r == HookResult::Changed) allowReturnOverride = true;
            }

            // --- Originál ---
            if (!skipOriginal && original)
            {
                if (!(allowReturnOverride && hook.returnOverridden))
                {
                    result = original(args...);
                }
            }

            // --- POST hooks ---
            for (auto& entry : hooksCopy)
            {
                if (entry.mode != HookMode::Post) continue;
                HookResult r = entry.handler(&hook, HookMode::Post);
                if (r == HookResult::Stop) break;
            }

            return result;
        }
        else
        {
            DynamicHook hook{argArray, argCount, self, nullptr};

            // --- PRE hooks ---
            for (auto& entry : hooksCopy)
            {
                if (entry.mode != HookMode::Pre) continue;
                HookResult r = entry.handler(&hook, HookMode::Pre);
                if (r == HookResult::Stop) return;
                if (r == HookResult::Handled)
                {
                    skipOriginal = true;
                    return;
                }
            }

            // --- Originál ---
            if (!skipOriginal && original)
            {
                original(args...);
            }

            // --- POST hooks ---
            for (auto& entry : hooksCopy)
            {
                if (entry.mode != HookMode::Post) continue;
                HookResult r = entry.handler(&hook, HookMode::Post);
                if (r == HookResult::Stop) break;
            }
        }
    }

    template <typename R, typename... Args>
    void RegisterSignatureDetour(
        const std::string& sigName,
        DynLibUtils::CModule module,
        R (**ppOriginal)(Args...),
        SignatureHandler handler,
        HookMode mode
    )
    {
        std::scoped_lock lock(signatureHooksMutex);

        auto it = signatureHooks.find(sigName);
        if (it != signatureHooks.end())
        {
            it->second.push_back({handler, mode});
            return;
        }

        void* addr = FindModuleSignature(std::move(module), sigName.c_str());
        if (!addr)
        {
            FP_ERROR("Failed to find signature for '{}'", sigName);
            return;
        }

        *ppOriginal = reinterpret_cast<R(*)(Args...)>(addr);
        SignatureStorage<R(*)(Args...)>::name = sigName;

        auto detour = [](Args... args) -> R
        {
            auto orig = SignatureStorage<R(*)(Args...)>::original;
            return DispatchSignatureCall(SignatureStorage<R(*)(Args...)>::name, orig, args...);
        };

        auto hook = funchook_create();
        using FnType = R(*)(Args...);

        if (funchook_prepare(
                hook,
                reinterpret_cast<void**>(ppOriginal),
                reinterpret_cast<void*>(static_cast<FnType>(detour))
            ) != 0 ||
            funchook_install(hook, 0) != 0)
        {
            FP_ERROR("Failed to hook '{}'", sigName);
            return;
        }

        SignatureStorage<R(*)(Args...)>::original = *ppOriginal;

        signatureHooks[sigName] = {};
        signatureHooks[sigName].push_back({handler, mode});
        hookHandles.push_back(hook);
    }

    HookResult DispatchConsoleListener(const CCommandContext& ctx, const CCommand& args, HookMode mode);

    bool DispatchGameEvent(IGameEvent* event, HookMode mode, bool& dontBroadcast);

    void DispatchEntitySpawn(CEntityInstance* entity);

    void DispatchEntityCreate(CEntityInstance* entity);

    void DispatchEntityDelete(CEntityInstance* entity);

    class CEntityListener : public IEntityListener
    {
        void OnEntitySpawned(CEntityInstance* pEntity) override { DispatchEntitySpawn(pEntity); }
        void OnEntityCreated(CEntityInstance* pEntity) override { DispatchEntityCreate(pEntity); }
        void OnEntityDeleted(CEntityInstance* pEntity) override { DispatchEntityDelete(pEntity); }

        void OnEntityParentChanged(CEntityInstance* pEntity, CEntityInstance* pNewParent) override
        {
        }
    };

    void HookSingleEntityOutput(CEntityInstance* entity,
                                const std::string& outputName,
                                EntityOutputHandler handler,
                                HookMode mode = HookMode::Post);

    void UnhookSingleEntityOutput(CEntityInstance* entity,
                                  const std::string& outputName,
                                  EntityOutputHandler handler,
                                  HookMode mode = HookMode::Post);

    HookResult DispatchEntityOutput(CEntityIOOutput* output,
                                    const char* name,
                                    CEntityInstance* activator,
                                    CEntityInstance* caller,
                                    const CVariant* value,
                                    float delay,
                                    HookMode mode);

    inline CEntityListener entityListener;

    inline CommandHandler WrapVoidHandler(
        const std::function<void(const CCommandContext&, const CCommand&, HookMode)>& fn)
    {
        return [fn](const CCommandContext& ctx, const CCommand& args, HookMode mode) -> HookResult
        {
            fn(ctx, args, mode);
            return HookResult::Continue;
        };
    }
}
