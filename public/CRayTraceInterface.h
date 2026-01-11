#pragma once
#include "vector.h"
#include "entityinstance.h"
#include "gametrace.h"
#include "trace.h"
#include "cmodel.h"
#include <optional>

#define RAYTRACE_INTERFACE_VERSION "CRayTraceInterface001"

class CBaseEntity;

enum class InteractionLayers : uint64_t
{
    Solid = 0x1,
    Hitboxes = 0x2,
    Trigger = 0x4,
    Sky = 0x8,
    PlayerClip = 0x10,
    NPCClip = 0x20,
    BlockLOS = 0x40,
    BlockLight = 0x80,
    Ladder = 0x100,
    Pickup = 0x200,
    BlockSound = 0x400,
    NoDraw = 0x800,
    Window = 0x1000,
    PassBullets = 0x2000,
    WorldGeometry = 0x4000,
    Water = 0x8000,
    Slime = 0x10000,
    TouchAll = 0x20000,
    Player = 0x40000,
    NPC = 0x80000,
    Debris = 0x100000,
    Physics_Prop = 0x200000,
    NavIgnore = 0x400000,
    NavLocalIgnore = 0x800000,
    PostProcessingVolume = 0x1000000,
    UnusedLayer3 = 0x2000000,
    CarriedObject = 0x4000000,
    PushAway = 0x8000000,
    ServerEntityOnClient = 0x10000000,
    CarriedWeapon = 0x20000000,
    StaticLevel = 0x40000000,
    csgo_team1 = 0x80000000,
    csgo_team2 = 0x100000000,
    csgo_grenadeclip = 0x200000000,
    csgo_droneclip = 0x400000000,
    csgo_moveable = 0x800000000,
    csgo_opaque = 0x1000000000,
    csgo_monster = 0x2000000000,
    csgo_thrown_grenade = 0x8000000000,
    FUNPLAY_IGNORE_PLAYER = (0x8000000000ull << 1)
};

inline InteractionLayers operator|(InteractionLayers a, InteractionLayers b)
{
    return static_cast<InteractionLayers>(
        static_cast<uint64_t>(a) | static_cast<uint64_t>(b)
    );
}

inline InteractionLayers& operator|=(InteractionLayers& a, InteractionLayers b)
{
    a = a | b;
    return a;
}

struct TraceOptions
{
    std::optional<InteractionLayers> InteractsWith{static_cast<InteractionLayers>(0x2c3011)};
    std::optional<InteractionLayers> InteractsExclude{};
    bool DrawBeam{false};
};

struct TraceResult
{
    Vector EndPos{};
    CEntityInstance* HitEntity{};
    float Fraction{};
    bool AllSolid{};
    Vector Normal{};
};

class CRayTraceInterface {
public:
    virtual ~CRayTraceInterface() = default;

    virtual std::optional<TraceResult> TraceShape(
        const Vector& origin,
        const QAngle& viewangles,
        CBaseEntity* ignorePlayer = nullptr,
        const TraceOptions* opts = nullptr) = 0;

    virtual std::optional<TraceResult> TraceEndShape(
        const Vector& origin,
        const Vector& endOrigin,
        CBaseEntity* ignorePlayer = nullptr,
        const TraceOptions* opts = nullptr) = 0;

    virtual std::optional<TraceResult> TraceShapeEx(
        const Vector& vecStart,
        const Vector& vecEnd,
        CTraceFilter& filterInc,
        Ray_t rayInc) = 0;
};