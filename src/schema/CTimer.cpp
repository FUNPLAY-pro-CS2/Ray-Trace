//
// Created by Michal Přikryl on 31.10.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "CTimer.h"

#include <Shared.h>

namespace
RayTracePlugin
{
    std::list<std::shared_ptr<CTimerBase>> g_timers;

    void RunTimers()
    {
        auto iterator = g_timers.begin();

        while (iterator != g_timers.end())
        {
            auto pTimer = *iterator;
            pTimer->Initialize();

            // Timer execute
            if (pTimer->GetLastExecute() + pTimer->GetInterval() <= universal_time && !pTimer->Execute(true))
                iterator = g_timers.erase(iterator);
            else
                iterator++;
        }
    }

    void RemoveAllTimers()
    {
        g_timers.clear();
    }

    void RemoveTimers(uint64 iTimerFlag)
    {
        auto iterator = g_timers.begin();

        while (iterator != g_timers.end())
            if ((*iterator)->IsTimerFlagSet(iTimerFlag))
                iterator = g_timers.erase(iterator);
            else
                iterator++;
    }

    std::weak_ptr<CTimer> CTimer::Create(float flInitialInterval, uint64 nTimerFlags, std::function<float()> func)
    {
        auto pTimer = std::make_shared<CTimer>(flInitialInterval, nTimerFlags, func, _timer_constructor_tag{});

        g_timers.push_back(pTimer);
        return pTimer;
    }

    bool CTimer::Execute(bool bAutomaticExecute)
    {
        SetInterval(m_func());
        SetLastExecute(universal_time);

        bool bContinue = GetInterval() >= 0;

        // Only scan the timer list if this isn't an automatic execute (RunTimers() already has the iterator to erase)
        if (!bAutomaticExecute && !bContinue)
            Cancel();

        return bContinue;
    }

    void CTimer::Cancel()
    {
        auto iterator = g_timers.begin();

        while (iterator != g_timers.end())
        {
            if (*iterator == shared_from_this())
            {
                g_timers.erase(iterator);
                break;
            }

            iterator++;
        }
    }
}
