//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include <cstdint>

namespace RayTracePlugin
{
    template <typename T, typename... Args>
    inline T CallVFunc(void* base, int index, Args... args)
    {
        using Fn = T(*)(void*, Args...);
        void** vtable = *reinterpret_cast<void***>(base);
        return reinterpret_cast<Fn>(vtable[index])(base, args...);
    }
}
