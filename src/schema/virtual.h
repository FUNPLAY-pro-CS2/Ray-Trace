//
// Created by Michal Přikryl on 20.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "platform.h"

#define CALL_VIRTUAL(retType, idx, ...) vmt::CallVirtual<retType>(idx, __VA_ARGS__)
#define CALL_VIRTUAL_OVERRIDE_VTBL(retType, idx, vtable, classPtr, ...) vmt::CallVirtualOverrideVTable<retType>(idx, vtable, classPtr, __VA_ARGS__)

namespace vmt {
    template <typename T = void*> inline T GetVMethod(uint32 uIndex, void* pClass)
    {
        if (!pClass)
        {
            return T();
        }

        void** pVTable = *static_cast<void***>(pClass);
        if (!pVTable)
        {
            return T();
        }

        return reinterpret_cast<T>(pVTable[uIndex]);
    }

    template <typename T, typename... Args> inline T CallVirtual(uint32 uIndex, void* pClass, Args... args)
    {
#ifdef _WIN32
        auto pFunc = GetVMethod<T(__thiscall*)(void*, Args...)>(uIndex, pClass);
#else
        auto pFunc = GetVMethod<T (*)(void*, Args...)>(uIndex, pClass);
#endif
        if (!pFunc)
        {
            return T();
        }

        return pFunc(pClass, args...);
    }

    template<typename T, typename... Args>
    inline T CallVirtualOverrideVTable(uint32 uIndex, void **pVTable, void *pClass, Args... args)
    {
        if (!pVTable)
        {
            Warning("Tried getting virtual function from a null vtable.\n");
            return T();
        }
#ifdef _WIN32
        auto pFunc = reinterpret_cast<T(__thiscall *)(void *, Args...)>(pVTable[uIndex]);
#else
        auto pFunc = reinterpret_cast<T(__cdecl *)(void *, Args...)>(pVTable[uIndex]);
#endif
        if (!pFunc)
        {
            Warning("Tried calling a null virtual function.\n");
            return T();
        }

        return pFunc(pClass, args...);
    }
} // namespace vmt