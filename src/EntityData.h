//
// Created by Michal Přikryl on 15.09.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <random>
#include <string>
#include "schema/CCSPlayerController.h"

namespace TemplatePlugin {
    enum class EntityType {
        None = 0,
    };

    template<typename T>
    struct EntityCustomData {
        T *Value = nullptr;

        explicit EntityCustomData(T *v = nullptr) : Value(v) {
        }
    };

    enum class ActionType_t {
        None = 0,
        EPush = 1,
        EPushPower = 2,
        IgnoreKnife = 3,
        FToPickup = 4,
        NoDoorFix = 5,
    };

    struct EntityData_t {
        EntityType Type = EntityType::None;

        std::function<void(CHandle<CEntityInstance>, CHandle<CCSPlayerController>)> OnTouch;
        std::function<void(CHandle<CEntityInstance>, CHandle<CCSPlayerController>)> OnUse;

        std::unique_ptr<EntityCustomData<void> > CustomData;

        CHandle<CCSPlayerController> GrabHolder = nullptr;
        float GrabDistance = 0.0f;

        CHandle<CBaseEntity> TouchedTo = nullptr;
        int NextSoundBlockTick = 0;

        ActionType_t ActionType = ActionType_t::None;

        bool SkinInitialized = false;

        void Init() {
            Type = EntityType::None;
            OnTouch = nullptr;
            OnUse = nullptr;
            CustomData.reset();
            GrabHolder = nullptr;
            GrabDistance = 0.0f;
            TouchedTo = nullptr;
            NextSoundBlockTick = 0;
            ActionType = ActionType_t::None;
            SkinInitialized = false;
        }
    };

    struct ActionEntity {
        std::string HammerId;
        int Number = 0;

        ActionEntity(const std::string &hammerId, int number)
            : HammerId(hammerId), Number(number) {
        }
    };

    struct HandleHasher {
        size_t operator()(const CHandle<CBaseEntity> &h) const noexcept {
            return std::hash<int>()(h.GetEntryIndex());
        }
    };

    struct HandleEqual {
        bool operator()(const CHandle<CBaseEntity> &a, const CHandle<CBaseEntity> &b) const noexcept {
            return a.GetEntryIndex() == b.GetEntryIndex();
        }
    };

    inline std::unordered_map<CHandle<CBaseEntity>, EntityData_t, HandleHasher, HandleEqual> EntityData;
}
