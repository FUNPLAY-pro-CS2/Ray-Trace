//
// Created by Michal Přikryl on 10.07.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "tasks.h"
#include <algorithm>
#include <chrono>
#include <mutex>
#include <queue>

namespace RayTracePlugin {
    double universal_time = 0.0;
    double last_tick_time = 0.0;
    double timer_next_think = 0.0;

    namespace {
        std::vector<Timer *> once_off_timers;
        std::vector<Timer *> repeat_timers;
        std::mutex nextFrameMutex;
        std::queue<std::function<void()> > nextFrameQueue;
    }
    void Tasks::NextFrame(std::function<void()> &&task) {
        std::lock_guard lock(nextFrameMutex);
        nextFrameQueue.emplace(std::move(task));
    }

    Timer::Timer(float interval, double execTime, TimerCallback callback, int flags)
        : Interval(interval), ExecTime(execTime), Callback(std::move(callback)), Flags(flags) {
    }

    void Tasks::Init() {
        universal_time = 0.0;
        last_tick_time = 0.0;
        timer_next_think = 0.0;
    }

    void Tasks::Shutdown() {
        for (auto *timer: once_off_timers)
            delete timer;
        for (auto *timer: repeat_timers)
            delete timer;
        once_off_timers.clear();
        repeat_timers.clear();
        std::lock_guard lock(nextFrameMutex);
        std::queue<std::function<void()> > empty;
        std::swap(nextFrameQueue, empty);
    }

    void Tasks::Tick(bool simulating) {
        std::queue<std::function<void()>> localQueue;
        {
            std::lock_guard lock(nextFrameMutex);
            std::swap(localQueue, nextFrameQueue);
        }

        while (!localQueue.empty()) {
            try {
                localQueue.front()();
            } catch (...) {
            }
            localQueue.pop();
        }

        double now = std::chrono::duration_cast<std::chrono::duration<float> >(
            std::chrono::steady_clock::now().time_since_epoch()).count();

        if (simulating)
            universal_time += now - last_tick_time;
        else
            universal_time += 0.015;

        last_tick_time = now;

        if (universal_time < timer_next_think)
            return;

        for (int i = static_cast<int>(once_off_timers.size()) - 1; i >= 0; --i) {
            Timer *timer = once_off_timers[i];
            if (universal_time >= timer->ExecTime) {
                timer->InExec = true;
                try {
                    timer->Callback();
                } catch (...) {
                }
                delete timer;
                once_off_timers.erase(once_off_timers.begin() + i);
            }
        }

        for (int i = static_cast<int>(repeat_timers.size()) - 1; i >= 0; --i) {
            Timer *timer = repeat_timers[i];
            if (universal_time >= timer->ExecTime) {
                timer->InExec = true;
                try {
                    timer->Callback();
                } catch (...) {
                }

                if (timer->KillMe) {
                    delete timer;
                    repeat_timers.erase(repeat_timers.begin() + i);
                    continue;
                }

                timer->InExec = false;
                timer->ExecTime = universal_time + timer->Interval;
            }
        }

        timer_next_think = universal_time + 0.1;
    }

    Timer *Tasks::AddTimer(float interval, TimerCallback callback, int flags) {
        Timer *timer = new Timer(interval, universal_time + interval, std::move(callback), flags);

        if (flags & TIMER_FLAG_REPEAT)
            repeat_timers.push_back(timer);
        else
            once_off_timers.push_back(timer);

        return timer;
    }

    void Tasks::KillTimer(Timer *timer) {
        if (!timer) return;

        auto killFrom = [](std::vector<Timer *> &list, Timer *target) {
            auto it = std::remove_if(list.begin(), list.end(), [=](Timer *t) { return t == target; });
            if (it != list.end()) {
                delete target;
                list.erase(it, list.end());
            }
        };

        if (timer->InExec) {
            timer->KillMe = true;
            return;
        }

        if (timer->Flags & TIMER_FLAG_REPEAT)
            killFrom(repeat_timers, timer);
        else
            killFrom(once_off_timers, timer);
    }

    void Tasks::RemoveMapChangeTimers() {
        auto removeFrom = [](std::vector<Timer*>& list) {
            for (int i = static_cast<int>(list.size()) - 1; i >= 0; --i) {
                Timer* t = list[i];
                if (t->Flags & TIMER_FLAG_NO_MAPCHANGE) {
                    delete t;
                    list.erase(list.begin() + i);
                }
            }
        };

        removeFrom(once_off_timers);
        removeFrom(repeat_timers);
    }
}
