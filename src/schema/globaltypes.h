//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once
#include <platform.h>
#include "soundflags.h"

// struct TransmitInfo
// {
// 	CBitVec<16384> *m_pTransmitEdict;
// };

namespace RayTracePlugin {
    enum CSRoundEndReason {
        TargetBombed = 1, /**< Target Successfully Bombed! */
        VIPEscaped, /**< The VIP has escaped! - Doesn't exist on CS:GO */
        VIPKilled, /**< VIP has been assassinated! - Doesn't exist on CS:GO */
        TerroristsEscaped, /**< The terrorists have escaped! */
        CTStoppedEscape, /**< The CTs have prevented most of the terrorists from escaping! */
        TerroristsStopped, /**< Escaping terrorists have all been neutralized! */
        BombDefused, /**< The bomb has been defused! */
        CTWin, /**< Counter-Terrorists Win! */
        TerroristWin, /**< Terrorists Win! */
        Draw, /**< Round Draw! */
        HostagesRescued, /**< All Hostages have been rescued! */
        TargetSaved, /**< Target has been saved! */
        HostagesNotRescued, /**< Hostages have not been rescued! */
        TerroristsNotEscaped, /**< Terrorists have not escaped! */
        VIPNotEscaped, /**< VIP has not escaped! - Doesn't exist on CS:GO */
        GameStart, /**< Game Commencing! */
        TerroristsSurrender, /**< Terrorists Surrender */
        CTSurrender, /**< CTs Surrender */
        TerroristsPlanted, /**< Terrorists Planted the bomb */
        CTsReachedHostage, /**< CTs Reached the hostage */
        SurvivalWin,
        SurvivalDraw
    };

    enum class CsTeam : uint8_t {
        None = 0,
        Spectator = 1,
        Terrorist = 2,
        CounterTerrorist = 3
    };

    enum GamePhase : int32_t {
        GAMEPHASE_WARMUP_ROUND,
        GAMEPHASE_PLAYING_STANDARD,
        GAMEPHASE_PLAYING_FIArenaT_HALF,
        GAMEPHASE_PLAYING_SECOND_HALF,
        GAMEPHASE_HALFTIME,
        GAMEPHASE_MATCH_ENDED,
        GAMEPHASE_MAX
    };

    enum InputBitMask_t : uint64_t {
        // MEnumeratorIsNotAFlag
        IN_NONE = 0x0,
        // MEnumeratorIsNotAFlag
        IN_ALL = 0xffffffffffffffff,
        IN_ATTACK = 0x1,
        IN_JUMP = 0x2,
        IN_DUCK = 0x4,
        IN_FORWARD = 0x8,
        IN_BACK = 0x10,
        IN_USE = 0x20,
        IN_TURNLEFT = 0x80,
        IN_TURNRIGHT = 0x100,
        IN_MOVELEFT = 0x200,
        IN_MOVERIGHT = 0x400,
        IN_ATTACK2 = 0x800,
        IN_RELOAD = 0x2000,
        IN_SPEED = 0x10000,
        IN_JOYAUTOSPRINT = 0x20000,
        // MEnumeratorIsNotAFlag
        IN_FIRST_MOD_SPECIFIC_BIT = 0x100000000,
        IN_USEORRELOAD = 0x100000000,
        IN_SCORE = 0x200000000,
        IN_ZOOM = 0x400000000,
        IN_LOOK_AT_WEAPON = 0x800000000,
    };

    enum EInButtonState : uint32_t {
        IN_BUTTON_UP = 0x0,
        IN_BUTTON_DOWN = 0x1,
        IN_BUTTON_DOWN_UP = 0x2,
        IN_BUTTON_UP_DOWN = 0x3,
        IN_BUTTON_UP_DOWN_UP = 0x4,
        IN_BUTTON_DOWN_UP_DOWN = 0x5,
        IN_BUTTON_DOWN_UP_DOWN_UP = 0x6,
        IN_BUTTON_UP_DOWN_UP_DOWN = 0x7,
        IN_BUTTON_STATE_COUNT = 0x8,
    };

    enum ParticleAttachment_t : uint32_t {
        PATTACH_INVALID = 0xffffffff,
        PATTACH_ABSORIGIN = 0x0, // Spawn at entity origin
        PATTACH_ABSORIGIN_FOLLOW = 0x1, // Spawn at and follow entity origin
        PATTACH_CUSTOMORIGIN = 0x2,
        PATTACH_CUSTOMORIGIN_FOLLOW = 0x3,
        PATTACH_POINT = 0x4, // Spawn at attachment point
        PATTACH_POINT_FOLLOW = 0x5, // Spawn at and follow attachment point
        PATTACH_EYES_FOLLOW = 0x6,
        PATTACH_OVERHEAD_FOLLOW = 0x7,
        PATTACH_WORLDORIGIN = 0x8,
        PATTACH_ROOTBONE_FOLLOW = 0x9,
        PATTACH_RENDERORIGIN_FOLLOW = 0xa,
        PATTACH_MAIN_VIEW = 0xb,
        PATTACH_WATERWAKE = 0xc,
        PATTACH_CENTER_FOLLOW = 0xd,
        PATTACH_CUSTOM_GAME_STATE_1 = 0xe,
        PATTACH_HEALTHBAR = 0xf,
        MAX_PATTACH_TYPES = 0x10,
    };

    enum ObserverMode_t : uint8_t {
        OBS_MODE_NONE = 0x0,
        OBS_MODE_FIXED = 0x1,
        OBS_MODE_IN_EYE = 0x2,
        OBS_MODE_CHASE = 0x3,
        OBS_MODE_ROAMING = 0x4,
        OBS_MODE_DIRECTED = 0x5,
        NUM_OBSERVER_MODES = 0x6,
    };

    typedef uint32 SoundEventGuid_t;

    struct SndOpEventGuid_t {
        SoundEventGuid_t m_nGuid;
        uint64 m_hStackHash;
    };

    // used with EmitSound_t
    enum gender_t : uint8 {
        GENDER_NONE = 0x0,
        GENDER_MALE = 0x1,
        GENDER_FEMALE = 0x2,
        GENDER_NAMVET = 0x3,
        GENDER_TEENGIRL = 0x4,
        GENDER_BIKER = 0x5,
        GENDER_MANAGER = 0x6,
        GENDER_GAMBLER = 0x7,
        GENDER_PRODUCER = 0x8,
        GENDER_COACH = 0x9,
        GENDER_MECHANIC = 0xA,
        GENDER_CEDA = 0xB,
        GENDER_CRAWLER = 0xC,
        GENDER_UNDISTRACTABLE = 0xD,
        GENDER_FALLEN = 0xE,
        GENDER_RIOT_CONTROL = 0xF,
        GENDER_CLOWN = 0x10,
        GENDER_JIMMY = 0x11,
        GENDER_HOSPITAL_PATIENT = 0x12,
        GENDER_BRIDE = 0x13,
        GENDER_LAST = 0x14,
    };

    enum DamageTypes_t : uint32_t {
        DMG_GENERIC = 0x0,
        DMG_CRUSH = 0x1,
        DMG_BULLET = 0x2,
        DMG_SLASH = 0x4,
        DMG_BURN = 0x8,
        DMG_VEHICLE = 0x10,
        DMG_FALL = 0x20,
        DMG_BLAST = 0x40,
        DMG_CLUB = 0x80,
        DMG_SHOCK = 0x100,
        DMG_SONIC = 0x200,
        DMG_ENERGYBEAM = 0x400,
        DMG_DROWN = 0x4000,
        DMG_POISON = 0x8000,
        DMG_RADIATION = 0x10000,
        DMG_DROWNRECOVER = 0x20000,
        DMG_ACID = 0x40000,
        DMG_PHYSGUN = 0x100000,
        DMG_DISSOLVE = 0x200000,
        DMG_BLAST_SURFACE = 0x400000,
        DMG_BUCKSHOT = 0x1000000,
        DMG_LASTGENERICFLAG = 0x1000000,
        DMG_HEADSHOT = 0x2000000,
        DMG_DANGERZONE = 0x4000000,
    };

    enum TakeDamageFlags_t : uint64_t
    {
        DFLAG_NONE = 0x0,
        DFLAG_SUPPRESS_HEALTH_CHANGES = 0x1,
        DFLAG_SUPPRESS_PHYSICS_FORCE = 0x2,
        DFLAG_SUPPRESS_EFFECTS = 0x4,
        DFLAG_PREVENT_DEATH = 0x8,
        DFLAG_FORCE_DEATH = 0x10,
        DFLAG_ALWAYS_GIB = 0x20,
        DFLAG_NEVER_GIB = 0x40,
        DFLAG_REMOVE_NO_RAGDOLL = 0x80,
        DFLAG_SUPPRESS_DAMAGE_MODIFICATION = 0x100,
        DFLAG_ALWAYS_FIRE_DAMAGE_EVENTS = 0x200,
        DFLAG_RADIUS_DMG = 0x400,
        DFLAG_FORCEREDUCEARMOR_DMG = 0x800,
        DFLAG_SUPPRESS_INTERRUPT_FLINCH = 0x1000,
        DFLAG_IGNORE_DESTRUCTIBLE_PARTS = 0x2000,
        DMG_LASTDFLAG = 0x2000,
        DFLAG_IGNORE_ARMOR = 0x4000,
        DFLAG_SUPPRESS_UTILREMOVE = 0x8000,
    };

    struct EmitSound_t {
        EmitSound_t() : m_nChannel(0),
                        m_pSoundName(0),
                        m_flVolume(VOL_NORM),
                        m_SoundLevel(SNDLVL_NONE),
                        m_nFlags(0),
                        m_nPitch(PITCH_NORM),
                        m_pOrigin(0),
                        m_flSoundTime(0.0f),
                        m_pflSoundDuration(0),
                        m_bEmitCloseCaption(true),
                        m_bWarnOnMissingCloseCaption(false),
                        m_bWarnOnDirectWaveReference(false),
                        m_nSpeakerEntity(-1),
                        m_nForceGuid(0),
                        m_SpeakerGender(GENDER_NONE) {
        }

        int m_nChannel;
        const char *m_pSoundName;
        float m_flVolume;
        soundlevel_t m_SoundLevel;
        int m_nFlags;
        int m_nPitch;
        const Vector *m_pOrigin;
        float m_flSoundTime;
        float *m_pflSoundDuration;
        bool m_bEmitCloseCaption;
        bool m_bWarnOnMissingCloseCaption;
        bool m_bWarnOnDirectWaveReference;
        CEntityIndex m_nSpeakerEntity;
        // CUtlVector<Vector, CUtlMemory<Vector, int> > m_UtlVecSoundOrigin;
        void *unk01;
        SoundEventGuid_t m_nForceGuid;
        gender_t m_SpeakerGender;
    };

    struct GameTick_t {
    public:
        DECLARE_SCHEMA_CLASS_INLINE(GameTick_t)

        SCHEMA_FIELD(int32_t, m_Value);
    };

    struct AmmoIndex_t {
    public:
        DECLARE_SCHEMA_CLASS_INLINE(AmmoIndex_t)

        SCHEMA_FIELD(int8_t, m_Value);
    };

    class CNetworkTransmitComponent {
    public:
        DECLARE_SCHEMA_CLASS_INLINE(CNetworkTransmitComponent)
    };

    class CNetworkVelocityVector {
    public:
        DECLARE_SCHEMA_CLASS_INLINE(CNetworkVelocityVector)

        SCHEMA_FIELD(float, m_vecX)

        SCHEMA_FIELD(float, m_vecY)

        SCHEMA_FIELD(float, m_vecZ)
    };

    class CNetworkOriginCellCoordQuantizedVector {
    public:
        DECLARE_SCHEMA_CLASS_INLINE(CNetworkOriginCellCoordQuantizedVector)

        SCHEMA_FIELD(uint16, m_cellX)

        SCHEMA_FIELD(uint16, m_cellY)

        SCHEMA_FIELD(uint16, m_cellZ)

        SCHEMA_FIELD(uint16, m_nOutsideWorld)

        // These are actually CNetworkedQuantizedFloat but we don't have the definition for it...
        SCHEMA_FIELD(float, m_vecX)

        SCHEMA_FIELD(float, m_vecY)

        SCHEMA_FIELD(float, m_vecZ)
    };

    class CInButtonState {
    public:
        DECLARE_SCHEMA_CLASS_INLINE(CInButtonState)

        // m_pButtonStates[3]

        // m_pButtonStates[0] is the mask of currently pressed buttons
        // m_pButtonStates[1] is the mask of buttons that changed in the current frame
        SCHEMA_FIELD_POINTER(uint64, m_pButtonStates)
    };

    enum class PlayerButtons : uint64_t {
        Attack = (1ull << 0),
        Jump = (1ull << 1),
        Duck = (1ull << 2),
        Forward = (1ull << 3),
        Back = (1ull << 4),
        Use = (1ull << 5),
        Cancel = (1ull << 6),
        Left = (1ull << 7),
        Right = (1ull << 8),
        Moveleft = (1ull << 9),
        Moveright = (1ull << 10),
        Attack2 = (1ull << 11),
        Run = (1ull << 12),
        Reload = (1ull << 13),
        Alt1 = (1ull << 14),
        Alt2 = (1ull << 15),
        Speed = (1ull << 16),
        Walk = (1ull << 17),
        Zoom = (1ull << 18),
        Weapon1 = (1ull << 19),
        Weapon2 = (1ull << 20),
        Bullrush = (1ull << 21),
        Grenade1 = (1ull << 22),
        Grenade2 = (1ull << 23),
        Attack3 = (1ull << 24),
        Scoreboard = (1ull << 33),
        Inspect = (1ull << 35),
    };

    inline std::string PlayerButtonsToString(uint64* buttons) {
        struct BtnName {
            uint64 flag;
            const char *name;
        };
        static constexpr BtnName names[] = {
            {(uint64) PlayerButtons::Attack, "Attack"},
            {(uint64) PlayerButtons::Jump, "Jump"},
            {(uint64) PlayerButtons::Duck, "Duck"},
            {(uint64) PlayerButtons::Forward, "Forward"},
            {(uint64) PlayerButtons::Back, "Back"},
            {(uint64) PlayerButtons::Use, "Use"},
            {(uint64) PlayerButtons::Cancel, "Cancel"},
            {(uint64) PlayerButtons::Left, "Left"},
            {(uint64) PlayerButtons::Right, "Right"},
            {(uint64) PlayerButtons::Moveleft, "Moveleft"},
            {(uint64) PlayerButtons::Moveright, "Moveright"},
            {(uint64) PlayerButtons::Attack2, "Attack2"},
            {(uint64) PlayerButtons::Run, "Run"},
            {(uint64) PlayerButtons::Reload, "Reload"},
            {(uint64) PlayerButtons::Alt1, "Alt1"},
            {(uint64) PlayerButtons::Alt2, "Alt2"},
            {(uint64) PlayerButtons::Speed, "Speed"},
            {(uint64) PlayerButtons::Walk, "Walk"},
            {(uint64) PlayerButtons::Zoom, "Zoom"},
            {(uint64) PlayerButtons::Weapon1, "Weapon1"},
            {(uint64) PlayerButtons::Weapon2, "Weapon2"},
            {(uint64) PlayerButtons::Bullrush, "Bullrush"},
            {(uint64) PlayerButtons::Grenade1, "Grenade1"},
            {(uint64) PlayerButtons::Grenade2, "Grenade2"},
            {(uint64) PlayerButtons::Attack3, "Attack3"},
            {(uint64) PlayerButtons::Scoreboard, "Scoreboard"},
            {(uint64) PlayerButtons::Inspect, "Inspect"},
        };

        std::string out;
        for (const auto &btn: names) {
            if (*buttons & btn.flag) {
                if (!out.empty())
                    out += " | ";
                out += btn.name;
            }
        }

        if (out.empty())
            out = "NONE";

        return out;
    }

    enum class PlayerFlags : uint32_t {
        OnGround     = (1u << 0), // At rest / on the ground
        Ducking      = (1u << 1), // Player is fully crouched
        WaterJump    = (1u << 2), // Jumping out of water
        OnTrain      = (1u << 3), // Controlling a train
        InRain       = (1u << 4), // Standing in rain
        Frozen       = (1u << 5), // Frozen for 3rd person cam
        AtControls   = (1u << 6), // Can't move, but key inputs go to other entity
        Client       = (1u << 7), // Is a player
        FakeClient   = (1u << 8), // Simulated server-side
        InWater      = (1u << 9), // In water
    };

    inline std::string PlayerFlagsToString(uint32_t flags) {
        struct FlagName {
            uint32_t flag;
            const char* name;
        };

        static constexpr FlagName names[] = {
            { (uint32_t)PlayerFlags::OnGround,   "OnGround" },
            { (uint32_t)PlayerFlags::Ducking,    "Ducking" },
            { (uint32_t)PlayerFlags::WaterJump,  "WaterJump" },
            { (uint32_t)PlayerFlags::OnTrain,    "OnTrain" },
            { (uint32_t)PlayerFlags::InRain,     "InRain" },
            { (uint32_t)PlayerFlags::Frozen,     "Frozen" },
            { (uint32_t)PlayerFlags::AtControls, "AtControls" },
            { (uint32_t)PlayerFlags::Client,     "Client" },
            { (uint32_t)PlayerFlags::FakeClient, "FakeClient" },
            { (uint32_t)PlayerFlags::InWater,    "InWater" },
        };

        std::string out;
        for (const auto& f : names) {
            if (flags & f.flag) {
                if (!out.empty())
                    out += " | ";
                out += f.name;
            }
        }

        if (out.empty())
            out = "NONE";

        return out;
    }

    class CGlowProperty {
    public:
        DECLARE_SCHEMA_CLASS_INLINE(CGlowProperty)

        SCHEMA_FIELD(Vector, m_fGlowColor)

        SCHEMA_FIELD(int, m_iGlowType)

        SCHEMA_FIELD(int, m_iGlowTeam)

        SCHEMA_FIELD(int, m_nGlowRange)

        SCHEMA_FIELD(int, m_nGlowRangeMin)

        SCHEMA_FIELD(Color, m_glowColorOverride)

        SCHEMA_FIELD(bool, m_bFlashing)

        SCHEMA_FIELD(bool, m_bGlowing)
    };

    struct TraceHistory {
        Vector start;
        Vector end;
        Ray_t ray;
        bool didHit;
        Vector m_vStartPos; // start position
        Vector m_vEndPos; // final position
        Vector m_vHitNormal; // surface normal at impact
        Vector m_vHitPoint; // exact hit point if m_bExactHitPoint is true, otherwise equal to m_vEndPos

        float m_flHitOffset; // surface normal hit offset
        float m_flFraction; // time completed, 1.0 = didn't hit anything

        float32 error;
        Vector velocity;
    };

    class CCheckTransmitInfoHack {
    public:
        CBitVec<16384> *m_pTransmitEntity;

    private:
        [[maybe_unused]] int8_t m_pad8[568];

    public:
        int32_t m_nPlayerSlot;
        bool m_bFullUpdate;
    };
}
