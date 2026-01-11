//
// Created by Michal Přikryl on 31.10.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <functional>
#include <list>
#include <memory>
#include <platform.h>
#include <Shared.h>

#include "tasks.h"

// clang-format off
#define TIMERFLAG_NONE		(0)
#define TIMERFLAG_MAP		(1 << 0) // Only valid for this map, cancels on map change
#define TIMERFLAG_ROUND		(1 << 1) // Only valid for this round, cancels on new round

namespace RayTracePlugin {
    class CTimerBase
    {
    protected:
        CTimerBase(float flInitialInterval, uint64 nTimerFlags) :
            m_flInterval(flInitialInterval), m_nTimerFlags(nTimerFlags)
        {}

        void SetInterval(float flInterval) { m_flInterval = flInterval; }
        void SetLastExecute(float flLastExecute) { m_flLastExecute = flLastExecute; }

    public:
        virtual bool Execute(bool bAutomaticExecute = false) = 0;
        virtual void Cancel() = 0;

        float GetInterval() { return m_flInterval; }
        float GetLastExecute() { return m_flLastExecute; }
        bool IsTimerFlagSet(uint64 iTimerFlag) { return !iTimerFlag || (m_nTimerFlags & iTimerFlag); }
        void Initialize()
        {
            if (m_flLastExecute == -1)
                m_flLastExecute = universal_time;
        }

    private:
        float m_flInterval;
        float m_flLastExecute = -1;
        uint64 m_nTimerFlags;
    };

    // Timer functions should return the time until next execution, or a negative value like -1.0f to stop
    // Having an interval of 0 is fine, in this case it will run on every game frame
    class CTimer : public CTimerBase, public std::enable_shared_from_this<CTimer>
    {
    private:
        // Silly workaround to achieve a "private constructor" only Create() can call
        struct _timer_constructor_tag
        {
            explicit _timer_constructor_tag() = default;
        };

    public:
        CTimer(float flInitialInterval, uint64 nTimerFlags, std::function<float()> func, _timer_constructor_tag) :
            CTimerBase(flInitialInterval, nTimerFlags), m_func(func)
        {}

        static std::weak_ptr<CTimer> Create(float flInitialInterval, uint64 nTimerFlags, std::function<float()> func);
        bool Execute(bool bAutomaticExecute) override;
        void Cancel() override;

    private:
        std::function<float()> m_func;
    };

    void RunTimers();
    void RemoveAllTimers();
    void RemoveTimers(uint64 iTimerFlag);
}
