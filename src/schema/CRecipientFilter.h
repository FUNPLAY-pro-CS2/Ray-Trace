//
// Created by Michal Přikryl on 17.08.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include "irecipientfilter.h"

class CRecipientFilter : public IRecipientFilter
{
public:
    CRecipientFilter(NetChannelBufType_t nBufType = BUF_RELIABLE, bool bInitMessage = false)
        : m_nBufType(nBufType), m_bInitMessage(bInitMessage)
    {
    }

    CRecipientFilter(const IRecipientFilter* source, CPlayerSlot exceptSlot = -1)
    {
        m_Recipients = source->GetRecipients();
        m_nBufType = source->GetNetworkBufType();
        m_bInitMessage = source->IsInitMessage();

        if (exceptSlot != -1)
            m_Recipients.Clear(exceptSlot.Get());
    }

    ~CRecipientFilter() override = default;

    NetChannelBufType_t GetNetworkBufType() const override { return m_nBufType; }
    bool IsInitMessage() const override { return m_bInitMessage; }
    const CPlayerBitVec& GetRecipients() const override { return m_Recipients; }
    CPlayerSlot GetPredictedPlayerSlot() const override { return m_slotPlayerExcludedDueToPrediction; }
    virtual CPlayerSlot GetExcludedPlayerDueToPrediction() const { return m_slotPlayerExcludedDueToPrediction; }

    void AddRecipient(CPlayerSlot slot)
    {
        if (slot.Get() >= 0 && slot.Get() < ABSOLUTE_PLAYER_LIMIT)
            m_Recipients.Set(slot.Get());
    }

    void AddRecipientsFromMask(uint64 mask)
    {
        for (int i = 0; i < 64; ++i)
        {
            if (mask & (uint64{1} << i))
                AddRecipient(CPlayerSlot(i));
        }
    }

    int GetRecipientCount() const
    {
        const auto& vec = GetRecipients();
        int count = 0;
        for (int i = 0; i < ABSOLUTE_PLAYER_LIMIT; ++i)
        {
            if (vec.Get(i))
                ++count;
        }
        return count;
    }

protected:
    CPlayerBitVec m_Recipients;
    CPlayerSlot m_slotPlayerExcludedDueToPrediction = -1;
    NetChannelBufType_t m_nBufType = BUF_DEFAULT;
    bool m_bInitMessage = false;
    bool m_bDoNotSuppressPrediction = false; // unused
};

class CSingleRecipientFilter : public CRecipientFilter
{
public:
    CSingleRecipientFilter(CPlayerSlot nRecipientSlot,
                           NetChannelBufType_t nBufType = BUF_RELIABLE,
                           bool bInitMessage = false)
        : CRecipientFilter(nBufType, bInitMessage)
    {
        if (nRecipientSlot.Get() >= 0 && nRecipientSlot.Get() < ABSOLUTE_PLAYER_LIMIT)
            m_Recipients.Set(nRecipientSlot.Get());
    }
};