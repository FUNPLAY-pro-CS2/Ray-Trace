//
// Created by Michal Přikryl on 21.09.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include "entitysystem.h"
#include "igamesystemfactory.h"

bool InitGameSystems();

class CGameSystem : public CBaseGameSystem
{
public:
    GS_EVENT(BuildGameSessionManifest);

    void Shutdown() override
    {
        delete sm_Factory;
    }

    void SetGameSystemGlobalPtrs(void* pValue) override
    {
        if (sm_Factory) sm_Factory->SetGlobalPtr(pValue);
    }

    bool DoesGameSystemReallocate() override { return sm_Factory->ShouldAutoAdd(); }

    static IGameSystemFactory* sm_Factory;
};