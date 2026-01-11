//
// Created by Michal Přikryl on 23.08.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <functional>
#include <igameevents.h>
#include <entitysystem.h>
#include <convar.h>
#include <string>

namespace TemplatePlugin
{
    class CBaseEntity;
    class CTakeDamageInfo;
    class CTakeDamageResult;

    enum class HookResult
    {
        Continue = 0,
        Changed = 1,
        Handled = 3,
        Stop = 4,
    };

    enum class HookMode
    {
        Pre,
        Post
    };

    struct DynamicHook
    {
        void** args;
        size_t argCount;
        void* self;
        void* returnValue;
        bool returnOverridden = false;

        template <typename T>
        T GetParam(size_t index) const
        {
            if (index >= argCount)
                throw std::out_of_range("arg index");

            if constexpr (std::is_pointer_v<T>)
            {
                return reinterpret_cast<T>(args[index]);
            }
            else
            {
                return *reinterpret_cast<T*>(args[index]);
            }
        }

        template <typename T>
        void SetParam(size_t index, const T& value)
        {
            if (index >= argCount)
                throw std::out_of_range("arg index");

            if constexpr (std::is_pointer_v<T>)
            {
                if constexpr (std::is_pointer_v<std::remove_pointer_t<T>>)
                {
                    **reinterpret_cast<T*>(args[index]) = *value;
                }
                else
                {
                    args[index] = const_cast<std::remove_const_t<T>>(value);
                }
            }
            else
            {
                *reinterpret_cast<T*>(args[index]) = value;
            }
        }

        template <typename T>
        T GetReturn() const
        {
            if (!returnValue)
                return T{};

            if constexpr (std::is_pointer_v<T>)
            {
                return *reinterpret_cast<T*>(returnValue);
            }
            else
            {
                return *reinterpret_cast<T*>(returnValue);
            }
        }

        template <typename T>
        void SetReturn(const T& value)
        {
            if (!returnValue)
                return;

            if constexpr (std::is_pointer_v<T>)
            {
                *reinterpret_cast<T*>(returnValue) = value;
            }
            else
            {
                *reinterpret_cast<T*>(returnValue) = value;
            }

            returnOverridden = true;
        }
    };

    struct EventOverride
    {
        bool dontBroadcast = false;
    };

    enum class EntityEventType
    {
        ENTITY_SPAWNED,
        ENTITY_CREATED,
        ENTITY_DELETED
    };

    struct EntityIOConnectionDesc_t
    {
        string_t m_targetDesc;
        string_t m_targetInput;
        string_t m_valueOverride;
        CEntityHandle m_hTarget;
        EntityIOTargetType_t m_nTargetType;
        int32 m_nTimesToFire;
        float m_flDelay;
    };

    struct EntityIOConnection_t : EntityIOConnectionDesc_t
    {
        bool m_bMarkedForRemoval;
        EntityIOConnection_t* m_pNext;
    };

    struct EntityIOOutputDesc_t
    {
        const char* m_pName;
        uint32 m_nFlags;
        uint32 m_nOutputOffset;
    };

    class CEntityIOOutput
    {
    public:
        void* vtable;
        EntityIOConnection_t* m_pConnections;
        EntityIOOutputDesc_t* m_pDesc;
    };

    using CommandHandler = std::function<HookResult(const CCommandContext&, const CCommand&, HookMode)>;
    using GameEventHandler = std::function<HookResult(IGameEvent* event, HookMode mode, EventOverride& override)>;
    using EntityEventHandler = void(*)(CEntityInstance*);
    using EntityParentChangedHandler = void(*)(CEntityInstance*, CEntityInstance*);
    using EntityOutputHandler = std::function<HookResult(CEntityIOOutput*, const char*, CEntityInstance*,
                                                         CEntityInstance*, const CVariant*, float)>;
    using SignatureHandler = std::function<HookResult(DynamicHook* hook, HookMode mode)>;
    using CBaseEntity_TakeDamageOld_t = int64_t(*)(CBaseEntity* pThis, CTakeDamageInfo* info, CTakeDamageResult* unk3);

    struct SignatureEntry
    {
        SignatureHandler handler;
        HookMode mode;

        SignatureEntry(SignatureHandler h, HookMode m)
            : handler(std::move(h)), mode(m)
        {
        }

        SignatureEntry(const SignatureEntry& other)
            : handler(other.handler), mode(other.mode)
        {
        }

        SignatureEntry& operator=(const SignatureEntry& other)
        {
            if (this != &other)
            {
                handler = other.handler;
                mode = other.mode;
            }
            return *this;
        }

        SignatureEntry(SignatureEntry&&) noexcept = default;

        SignatureEntry& operator=(SignatureEntry&&) noexcept = default;
    };

    template <typename T>
    struct SignatureStorage;

    template <typename R, typename... Args>
    struct SignatureStorage<R(*)(Args...)>
    {
        static inline std::string name;

        static inline R (*original)(Args...) = nullptr;
    };
}
