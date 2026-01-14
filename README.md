# Ray-Trace

**Shared ray tracing interface for Metamod:source & CounterStrikeSharp
plugins**

------------------------------------------------------------------------

## Overview

`Ray-Trace` is a lightweight **Metamod interface module** for\
**Counter-Strike 2** servers.

It exposes a shared interface:

    CRayTraceInterface001

which can be consumed from:

-   Native **Metamod C++ plugins**
-   Managed **CounterStrikeSharp C# plugins**

The goal is to provide a **single tracing backend** usable from both
worlds\
without duplicating engine detours.

------------------------------------------------------------------------

## Features

-   Metamod meta interface (`CRayTraceInterface001`)
-   Works in **C++ and C#**
-   Physics / hitbox / world trace presets
-   Custom collision masks
-   Optional debug beam rendering
-   Zero configuration
-   Ultra low overhead

------------------------------------------------------------------------

## Exposed Interface

``` cpp
class CRayTraceInterface
{
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
```

------------------------------------------------------------------------

## Getting the interface

### C++ (Metamod plugin)

``` cpp
int ret = 0;

auto* rayTraceIface = static_cast<CRayTraceInterface*>(
    g_SMAPI->MetaFactory("CRayTraceInterface001", &ret, nullptr)
);

if (ret == META_IFACE_FAILED || !rayTraceIface)
{
    FP_ERROR("Failed to lookup Ray-Trace interface!");
}
```

### C# (CounterStrikeSharp plugin)

``` csharp
private nint _handle;

public override void Load(bool hotReload)
{
    _handle = Utilities.MetaFactory("CRayTraceInterface001");

    if (_handle == nint.Zero)
    {
        throw new Exception("Failed to get Ray-Trace interface handle");
    }
}
```

------------------------------------------------------------------------

## Calling methods from C++ (Metamod)

On the native side you simply call the interface methods directly
(no vtable binding required).

### Basic usage

```cpp
CRayTraceInterface* g_RayTrace = nullptr;

bool InitRayTrace()
{
    int ret = 0;

    g_RayTrace = static_cast<CRayTraceInterface*>(
        g_SMAPI->MetaFactory("CRayTraceInterface001", &ret, nullptr)
    );

    if (ret == META_IFACE_FAILED || !g_RayTrace)
    {
        FP_ERROR("Failed to lookup Ray-Trace interface!");
        return false;
    }

    return true;
}
```

---

### Calling TraceShape

```cpp
TraceOptions opts;
opts.InteractsWith = MASK_SHOT_FULL;
opts.DrawBeam = true;

auto result = g_RayTrace->TraceShape(
    origin,
    viewAngles,
    ignorePlayer,
    &opts
);

if (result.has_value())
{
    auto& tr = result.value();

    Msg("Hit fraction: %f\n", tr.Fraction);
    Msg("End pos: %f %f %f\n",
        tr.EndPos.x,
        tr.EndPos.y,
        tr.EndPos.z);
}
```

---

### Calling TraceEndShape

```cpp
auto result = g_RayTrace->TraceEndShape(
    startPos,
    endPos,
    ignorePlayer,
    &opts
);
```

---

### Calling TraceShapeEx (low-level)

```cpp
Ray_t ray;
CTraceFilter filter(
    static_cast<uint64_t>(MASK_SHOT_FULL),
    COLLISION_GROUP_DEFAULT,
    true
);

auto result = g_RayTrace->TraceShapeEx(
    startPos,
    endPos,
    filter,
    ray
);
```

------------------------------------------------------------------------

## Calling methods from C# (CounterStrikeSharp plugin)

``` csharp
private Func<nint, nint, nint, nint, nint, nint>? _traceShape;
private Func<nint, nint, nint, nint, nint, nint>? _traceEndShape;
private Func<nint, nint, nint, nint, nint, nint>? _traceShapeEx;

private void Bind()
{
    // index 1 - TraceShape
    _traceShape = VirtualFunction.Create<
        nint, // this
        nint, // Vector*
        nint, // QAngle*
        nint, // ignore entity
        nint, // TraceOptions*
        nint
    >(_handle, 1);

    // index 2 - TraceEndShape
    _traceEndShape = VirtualFunction.Create<
        nint, // this
        nint, // start Vector*
        nint, // end Vector*
        nint, // ignore entity
        nint, // TraceOptions*
        nint
    >(_handle, 2);

    // index 3 - TraceShapeEx
    _traceShapeEx = VirtualFunction.Create<
        nint, // this
        nint, // start Vector*
        nint, // end Vector*
        nint, // CTraceFilter*
        nint, // Ray_t*
        nint
    >(_handle, 3);
}

public nint TraceShape(
    nint origin,
    nint angles,
    nint ignoreEntity,
    nint options)
{
    return _traceShape!(
        _handle,
        origin,
        angles,
        ignoreEntity,
        options
    );
}

public nint TraceEndShape(
    nint start,
    nint end,
    nint ignoreEntity,
    nint options)
{
    return _traceEndShape!(
        _handle,
        start,
        end,
        ignoreEntity,
        options
    );
}

public nint TraceShapeEx(
    nint start,
    nint end,
    nint filter,
    nint ray)
{
    return _traceShapeEx!(
        _handle,
        start,
        end,
        filter,
        ray
    );
}
```

------------------------------------------------------------------------

## Collision masks (C#)

``` csharp
using System;

[Flags]
public enum InteractionLayers : ulong
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
    FUNPLAY_IGNORE_PLAYER = (0x8000000000ul << 1)
}

public static class TraceMasks
{
    public const InteractionLayers MASK_SHOT_PHYSICS =
        InteractionLayers.Solid |
        InteractionLayers.PlayerClip |
        InteractionLayers.Window |
        InteractionLayers.PassBullets |
        InteractionLayers.Player |
        InteractionLayers.NPC |
        InteractionLayers.Physics_Prop;

    public const InteractionLayers MASK_SHOT_HITBOX =
        InteractionLayers.Hitboxes |
        InteractionLayers.Player |
        InteractionLayers.NPC;

    public const InteractionLayers MASK_SHOT_FULL =
        MASK_SHOT_PHYSICS |
        InteractionLayers.Hitboxes;

    public const InteractionLayers MASK_WORLD_ONLY =
        InteractionLayers.Solid |
        InteractionLayers.Window |
        InteractionLayers.PassBullets;

    public const InteractionLayers MASK_GRENADE =
        InteractionLayers.Solid |
        InteractionLayers.Window |
        InteractionLayers.Physics_Prop |
        InteractionLayers.PassBullets;

    public const InteractionLayers MASK_PLAYER_MOVE =
        InteractionLayers.Solid |
        InteractionLayers.Window |
        InteractionLayers.PlayerClip |
        InteractionLayers.PassBullets;

    public const InteractionLayers MASK_NPC_MOVE =
        InteractionLayers.Solid |
        InteractionLayers.Window |
        InteractionLayers.NPCClip |
        InteractionLayers.PassBullets;
}
```

------------------------------------------------------------------------

## Directory layout

    addons/
    └── Ray-Trace/
        └── bin/linuxsteamrt64/Ray-Trace.so

------------------------------------------------------------------------

## Build

### Requirements

-   HL2SDK-CS2
-   Metamod:Source
-   CMake

``` bash
git clone https://github.com/FUNPLAY-pro-CS2/Ray-Trace.git
cd Ray-Trace
git submodule update --init --recursive
docker compose -f docker/docker-compose.yml up
```

------------------------------------------------------------------------

## License

GPLv3

------------------------------------------------------------------------

## Author

**Michal "Slynx" Přikryl**\
https://slynxdev.cz
