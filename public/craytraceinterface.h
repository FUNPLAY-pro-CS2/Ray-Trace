#pragma once
#include "transform.h"
#include "vector.h"
#include "gametrace.h"
#include "entityinstance.h"

#ifdef MASK_SHOT
#undef MASK_SHOT
#endif

#define RAYTRACE_INTERFACE_VERSION "CRayTraceInterface002"

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
    csgo_thrown_grenade = 0x8000000000
};

constexpr InteractionLayers operator|(InteractionLayers a, InteractionLayers b)
{
    return static_cast<InteractionLayers>(
        static_cast<uint64_t>(a) | static_cast<uint64_t>(b)
    );
}

constexpr InteractionLayers operator&(InteractionLayers a, InteractionLayers b)
{
    return static_cast<InteractionLayers>(
        static_cast<uint64_t>(a) & static_cast<uint64_t>(b)
    );
}

constexpr InteractionLayers operator~(InteractionLayers a)
{
    return static_cast<InteractionLayers>(
        ~static_cast<uint64_t>(a)
    );
}

constexpr InteractionLayers& operator|=(InteractionLayers& a, InteractionLayers b)
{
    a = a | b;
    return a;
}

constexpr InteractionLayers MASK_SHOT =
    InteractionLayers::Solid |
    InteractionLayers::Player |
    InteractionLayers::NPC |
    InteractionLayers::Window |
    InteractionLayers::Debris |
    InteractionLayers::Hitboxes;

/// Custom base (0x2C3011, using this as default in my plugins)
constexpr InteractionLayers MASK_SHOT_PHYSICS =
    InteractionLayers::Solid |
    InteractionLayers::PlayerClip |
    InteractionLayers::Window |
    InteractionLayers::PassBullets |
    InteractionLayers::Player |
    InteractionLayers::NPC |
    InteractionLayers::Physics_Prop;

/// Only hitboxes (headshots etc.)
constexpr InteractionLayers MASK_SHOT_HITBOX =
    InteractionLayers::Hitboxes |
    InteractionLayers::Player |
    InteractionLayers::NPC;

/// Physics + hitboxes (full bullet trace)
constexpr InteractionLayers MASK_SHOT_FULL =
    MASK_SHOT_PHYSICS |
    InteractionLayers::Hitboxes;

/// World only (no entities)
constexpr InteractionLayers MASK_WORLD_ONLY =
    InteractionLayers::Solid |
    InteractionLayers::Window |
    InteractionLayers::PassBullets;

/// Grenade trace
constexpr InteractionLayers MASK_GRENADE =
    InteractionLayers::Solid |
    InteractionLayers::Window |
    InteractionLayers::Physics_Prop |
    InteractionLayers::PassBullets;

/// Brush only
constexpr InteractionLayers MASK_BRUSH_ONLY =
    InteractionLayers::Solid |
    InteractionLayers::Window;

/// Movement (player)
constexpr InteractionLayers MASK_PLAYER_MOVE =
    InteractionLayers::Solid |
    InteractionLayers::Window |
    InteractionLayers::PlayerClip |
    InteractionLayers::PassBullets;

/// Movement (NPC)
constexpr InteractionLayers MASK_NPC_MOVE =
    InteractionLayers::Solid |
    InteractionLayers::Window |
    InteractionLayers::NPCClip |
    InteractionLayers::PassBullets;

static_assert(
    static_cast<uint64_t>(MASK_SHOT_PHYSICS) == 0x2c3011,
    "MASK_SHOT_PHYSICS mismatch!"
);

struct TraceOptions
{
    uint64_t InteractsAs = 0;
    uint64_t InteractsWith = static_cast<uint64_t>(MASK_SHOT_PHYSICS);
    uint64_t InteractsExclude = 0;
    int DrawBeam = 0;
};

struct TraceResult
{
    // Start & end positions
    float StartPosX, StartPosY, StartPosZ;
    float EndPosX, EndPosY, EndPosZ;
    float HitPointX, HitPointY, HitPointZ;

    // Hit normal
    float NormalX, NormalY, NormalZ;

    // Fraction & hit offset
    float Fraction;
    float HitOffset;

    // Hit triangle / hitbox / hitgroup
    int TriangleIndex;       // index of triangle hit
    int HitboxBoneIndex;     // bone index of hitbox

    // Contents / surface flags
    int Contents;            // contents of the hit

    // Ray type
    int RayType;

    // Start in solid & exact hit
    bool AllSolid;
    bool ExactHitPoint;

    uintptr_t HitEntity;     
    uintptr_t Hitbox;        
    uintptr_t SurfaceProps;  
    uintptr_t BodyHandle;    
    uintptr_t ShapeHandle;
    CTransform BodyTransform;
    RnCollisionAttr_t ShapeAttributes;
};



class CRayTraceInterface
{
public:
    virtual ~CRayTraceInterface() = default;

    virtual bool TraceShape(
        const Vector* pVecStart,
        const QAngle* pAngAngles,
        CEntityInstance* pIgnoreEntity,
        TraceOptions* pTraceOptions,
        TraceResult* pTraceResult
    ) = 0;

    virtual bool TraceEndShape(
        const Vector* pVecStart,
        const Vector* pVecEnd,
        CEntityInstance* pIgnoreEntity,
        TraceOptions* pTraceOptions,
        TraceResult* pTraceResult
    ) = 0;

    virtual bool TraceHullShape(
        const Vector* pVecStart,
        const Vector* pVecEnd,
        const Vector* pVecMins,
        const Vector* pVecMaxs,
        CEntityInstance* pIgnoreEntity,
        TraceOptions* pTraceOptions,
        TraceResult* pTraceResult
    ) = 0;

    virtual bool TraceShapeEx(
        const Vector* pVecStart,
        const Vector* pVecEnd,
        CTraceFilter* pTraceFilter,
        Ray_t* pRay,
        TraceResult* pTraceResult
    ) = 0;
};
