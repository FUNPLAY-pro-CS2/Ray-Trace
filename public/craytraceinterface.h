#pragma once
#include "transform.h"
#include "vector.h"
#include "gametrace.h"
#include "entityinstance.h"

#define RAYTRACE_INTERFACE_VERSION "CRayTraceInterface001"

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
    TraceResult()
    {
        m_flFraction = 1.0f;
        m_bStartInSolid = false;
        m_bExactHitPoint = false;
        m_pEnt = nullptr;
        m_pHitbox = nullptr;
        m_pSurfaceProperties = nullptr;
    }

    explicit TraceResult(const CGameTrace* pTrace, bool bResult)
    {
        m_bTraceShapeResult = bResult;
        m_pSurfaceProperties = pTrace->m_pSurfaceProperties;
        m_pEnt = pTrace->m_pEnt;
        m_pHitbox = pTrace->m_pHitbox;
        m_hBody = pTrace->m_hBody;
        m_hShape = pTrace->m_hShape;
        m_nContents = pTrace->m_nContents;
        m_BodyTransform = pTrace->m_BodyTransform;
        m_ShapeAttributes = pTrace->m_ShapeAttributes;
        m_vStartPos = pTrace->m_vStartPos;
        m_vEndPos = pTrace->m_vEndPos;
        m_vHitNormal = pTrace->m_vHitNormal;
        m_vHitPoint = pTrace->m_vHitPoint;
        m_flHitOffset = pTrace->m_flHitOffset;
        m_flFraction = pTrace->m_flFraction;
        m_nTriangle = pTrace->m_nTriangle;
        m_nHitboxBoneIndex = pTrace->m_nHitboxBoneIndex;
        m_eRayType = pTrace->m_eRayType;
        m_bStartInSolid = pTrace->m_bStartInSolid;
        m_bExactHitPoint = pTrace->m_bExactHitPoint;
    }

    TraceResult(const TraceResult&) = delete;
    TraceResult& operator=(const TraceResult&) = delete;

    TraceResult(TraceResult&&) = default;
    TraceResult& operator=(TraceResult&&) = default;

    const Vector& StartPos() const { return m_vStartPos; }
    const Vector& EndPos() const { return m_vEndPos; }
    const Vector& HitPoint() const { return m_vHitPoint; }
    const Vector& Normal() const { return m_vHitNormal; }

    float Fraction() const { return m_flFraction; }
    float HitOffset() const { return m_flHitOffset; }

    bool DidHit() const { return m_flFraction < 1.0f; }
    bool IsAllSolid() const { return m_bStartInSolid; }
    bool HasExactHit() const { return m_bExactHitPoint; }

    CEntityInstance* HitEntity() const { return m_pEnt; }

    int TriangleIndex() const { return m_nTriangle; }
    int HitboxBoneIndex() const { return m_nHitboxBoneIndex; }

    CHitBox* Hitbox() const { return const_cast<CHitBox*>(m_pHitbox); }

    int Contents() const { return m_nContents; }
    RayType_t RayType() const { return m_eRayType; }

    IPhysicsBody* Body() const { return m_hBody; }
    IPhysicsShape* Shape() const { return m_hShape; }

    const CTransform& BodyTransform() const { return m_BodyTransform; }
    const RnCollisionAttr_t& ShapeAttributes() const { return m_ShapeAttributes; }

    CPhysSurfaceProperties* Surface() const { return const_cast<CPhysSurfaceProperties*>(m_pSurfaceProperties); }

private:
    bool m_bTraceShapeResult;           // return value from RayTrace::TraceShapeEx

    const CPhysSurfaceProperties *m_pSurfaceProperties;
    CEntityInstance *m_pEnt;
    const CHitBox *m_pHitbox;

    HPhysicsBody m_hBody;
    HPhysicsShape m_hShape;

    uint64 m_nContents;					// contents on other side of surface hit

    CTransform m_BodyTransform;
    RnCollisionAttr_t m_ShapeAttributes;

    Vector m_vStartPos; 				// start position
    Vector m_vEndPos; 					// final position
    Vector m_vHitNormal; 				// surface normal at impact
    Vector m_vHitPoint;					// exact hit point if m_bExactHitPoint is true, otherwise equal to m_vEndPos

    float m_flHitOffset;				// surface normal hit offset
    float m_flFraction;					// time completed, 1.0 = didn't hit anything

    int32 m_nTriangle;					// the index of the triangle that was hit
    int16 m_nHitboxBoneIndex; 			// the index of the hitbox bone that was hit

    RayType_t m_eRayType;

    bool m_bStartInSolid;				// if true, the initial point was in a solid area
    bool m_bExactHitPoint;				// if true, then m_vHitPoint is the exact hit point of the query and the shape
};

class CRayTraceInterface
{
public:
    virtual ~CRayTraceInterface() = default;

    virtual TraceResult TraceShape(const Vector& vecStart,
                                   const QAngle& angAngles,
                                   CEntityInstance* pIgnoreEntity,
                                   TraceOptions* pTraceOptions) = 0;

    virtual TraceResult TraceEndShape(const Vector& vecStart,
                                      const Vector& vecEnd,
                                      CEntityInstance* pIgnoreEntity,
                                      TraceOptions* pTraceOptions) = 0;

    virtual TraceResult TraceHullShape(const Vector& vecStart,
                                       const Vector& vecEnd,
                                       const Vector& vecMins,
                                       const Vector& vecMaxs,
                                       CEntityInstance* pIgnoreEntity,
                                       TraceOptions* pTraceOptions) = 0;

    virtual TraceResult TraceShapeEx(const Vector& vecStart,
                                     const Vector& vecEnd,
                                     CTraceFilter* pTraceFilter,
                                     Ray_t* pRay) = 0;
};
