# Ray-Trace

**Shared ray tracing interface for Metamod:Source & CounterStrikeSharp
plugins**

------------------------------------------------------------------------

## Overview

`Ray-Trace` is a lightweight **Metamod interface module** for\
**Counter-Strike 2** servers.

It exposes a shared interface: `CRayTraceInterface001` which can be
consumed from:

-   Native **Metamod C++ plugins**
-   Managed **CounterStrikeSharp C# plugins**

The interface is implemented as a **C++ virtual class** and is accessed
from C# by calling its **vtable functions directly** using a native
handle.

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
-   ABI-safe (no STL types in interface)
-   Virtual destructor for safe lifetime management

------------------------------------------------------------------------

## Exposed Interface (C++)

``` cpp
class CRayTraceInterface
{
public:
    virtual ~CRayTraceInterface() = default;

    virtual bool TraceShape(
        const Vector* pOrigin,
        const QAngle* pViewAngles,
        CBaseEntity* pIgnorePlayer,
        const TraceOptions* pOpts,
        TraceResult* pOutResult
    ) = 0;

    virtual bool TraceEndShape(
        const Vector* pOrigin,
        const Vector* pEndOrigin,
        CBaseEntity* pIgnorePlayer,
        const TraceOptions* pOpts,
        TraceResult* pOutResult
    ) = 0;

    virtual bool TraceShapeEx(
        const Vector* pVecStart,
        const Vector* pVecEnd,
        CTraceFilter* pFilterInc,
        Ray_t rayInc,
        TraceResult* pOutResult
    ) = 0;
};
```

**Return value:** - true → trace hit something, TraceResult is valid\
- false → no hit

------------------------------------------------------------------------

## Getting the interface

**C++ (Metamod plugin)**

``` cpp
CRayTraceInterface* g_pRayTrace = nullptr;
bool g_bRayTraceLoaded = false;

bool LoadRayTrace()
{
    int iRet = 0;

    g_pRayTrace = static_cast<CRayTraceInterface*>(
        g_SMAPI->MetaFactory("CRayTraceInterface001", &iRet, nullptr)
    );

    if (iRet == META_IFACE_FAILED || !g_pRayTrace)
    {
        FP_ERROR("Failed to lookup Ray-Trace interface!");
        return false;
    }

    g_bRayTraceLoaded = true;
    return true;
}
```

**C# (CounterStrikeSharp plugin)**

``` csharp
private nint g_pRayTraceHandle = nint.Zero;
private bool g_bRayTraceLoaded = false;

public override void Load(bool hotReload)
{
    g_pRayTraceHandle = Utilities.MetaFactory("CRayTraceInterface001");

    if (g_pRayTraceHandle == nint.Zero)
    {
        throw new Exception("Failed to get Ray-Trace interface handle");
    }

    Bind();
    g_bRayTraceLoaded = true;
}
```

The returned handle is a pointer to the native CRayTraceInterface
object.

------------------------------------------------------------------------

## Calling methods from C++ (Metamod)

**TraceShape example**

``` cpp
Vector vecOrigin{};
QAngle angView{};
TraceOptions traceOpts{};
traceOpts.InteractsWith = static_cast<uint64_t>(MASK_SHOT_FULL);
traceOpts.DrawBeam = 1;

TraceResult traceResult{};

if (g_pRayTrace && g_bRayTraceLoaded)
{
    bool bHit = g_pRayTrace->TraceShape(
        &vecOrigin,
        &angView,
        nullptr,
        &traceOpts,
        &traceResult
    );

    if (bHit)
    {
        Msg("Hit fraction: %f\n", traceResult.Fraction);
        Msg("End pos: %f %f %f\n",
            traceResult.EndPos.x,
            traceResult.EndPos.y,
            traceResult.EndPos.z);
    }
}
```

**TraceEndShape example**

``` cpp
TraceResult traceResult{};

bool bHit = g_pRayTrace->TraceEndShape(
    &vecStartPos,
    &vecEndPos,
    nullptr,
    &traceOpts,
    &traceResult
);
```

**TraceShapeEx (low-level)**

``` cpp
Ray_t ray{};
CTraceFilter filter(
    static_cast<uint64_t>(MASK_SHOT_FULL),
    COLLISION_GROUP_DEFAULT,
    true
);

TraceResult traceResult{};

bool bHit = g_pRayTrace->TraceShapeEx(
    &vecStartPos,
    &vecEndPos,
    &filter,
    ray,
    &traceResult
);
```

------------------------------------------------------------------------

## Calling methods from C# (CounterStrikeSharp plugin)

``` csharp
private delegate bool TraceShapeFn(
    nint pThis,
    nint pOrigin,
    nint pAngles,
    nint pIgnoreEntity,
    nint pOptions,
    nint pOutResult
);

private TraceShapeFn? _traceShape;
private TraceShapeFn? _traceEndShape;
private TraceShapeFn? _traceShapeEx;

private void Bind()
{
    _traceShape = VirtualFunction.Create<TraceShapeFn>(g_pRayTraceHandle, 1);
    _traceEndShape = VirtualFunction.Create<TraceShapeFn>(g_pRayTraceHandle, 2);
    _traceShapeEx = VirtualFunction.Create<TraceShapeFn>(g_pRayTraceHandle, 3);
}

public bool TraceShape(
    nint pOrigin,
    nint pAngles,
    nint pIgnoreEntity,
    nint pOptions,
    nint pOutResult)
{
    if (!g_bRayTraceLoaded || g_pRayTraceHandle == nint.Zero)
        return false;

    return _traceShape!(
        g_pRayTraceHandle,
        pOrigin,
        pAngles,
        pIgnoreEntity,
        pOptions,
        pOutResult
    );
}
```

------------------------------------------------------------------------

## Memory allocation from C# (Important)

When calling `TraceShape` or `TraceEndShape` from C#, the plugin **must
allocate native memory** for the following structures:

-   `TraceOptions`
-   `TraceResult`

These parameters are native pointers in C++ (`TraceOptions*` and
`TraceResult*`) and must remain valid for the duration of the call.

The recommended and safest approach is using **stackalloc** (or unsafe
stack variables) to provide native memory on the stack.

Failing to allocate valid memory for these parameters will result in
crashes or undefined behavior.

### Example (C# stackalloc)

``` csharp
[StructLayout(LayoutKind.Sequential, Pack = 8)]
public struct TraceOptions
{
    public ulong InteractsWith;
    public ulong InteractsExclude;
    public int DrawBeam;
}

[StructLayout(LayoutKind.Sequential, Pack = 8)]
public struct TraceResult
{
    public Vector EndPos;
    public nint HitEntity;
    public float Fraction;
    public int AllSolid;
    public Vector Normal;
}

unsafe
{
    Vector origin = player.Position;
    QAngle angles = player.ViewAngles;

    TraceOptions* opts = stackalloc TraceOptions[1];
    opts->InteractsWith = (ulong)MASK_SHOT_FULL;
    opts->InteractsExclude = 0;
    opts->DrawBeam = 0;

    TraceResult* result = stackalloc TraceResult[1];

    bool hit = _traceShape!(
        g_pRayTraceHandle,
        (nint)&origin,
        (nint)&angles,
        nint.Zero,
        (nint)opts,
        (nint)result
    );

    if (hit)
    {
        Console.WriteLine($"Hit at: {result->EndPos}");
    }
}
```

------------------------------------------------------------------------

## Low-level usage from C# (Ray_t & CTraceFilter)

When using the low-level method `TraceShapeEx` from a C# plugin, the
plugin must provide its own native-compatible implementations of the
following engine structures:

-   `Ray_t`
-   `CTraceFilter`

These structures are not exposed directly by the Ray-Trace interface and
must be recreated in managed code with correct memory layout.

### Ray_t (C#)

The C# plugin must define a struct that matches the native `Ray_t`
layout used by the engine.

Example (simplified):

``` csharp
[StructLayout(LayoutKind.Sequential)]
public struct Ray_t
{
    public Vector3 m_vecStart;
    public Vector3 m_vecDelta;
    public byte m_IsRay;
    public byte m_IsSwept;
}
```

(Exact layout depends on the engine version and must match native
memory.)

**CTraceFilter (C#)**\
For `CTraceFilter`, the plugin must:

-   Define a managed struct matching the native layout.
-   Resolve the CTraceFilter vtable pointer using a signature scan.
-   Assign the resolved vtable to the struct before calling
    TraceShapeEx.

Example concept:

``` csharp
[StructLayout(LayoutKind.Sequential)]
public unsafe struct CTraceFilter
{
    public nint __vtable;
    public ulong m_nInteractsWith;
    public T ...;
}
```

**Important notes** - This setup is only required when using the
low-level API: - TraceShapeEx(...) - High-level functions (TraceShape,
TraceEndShape) do not require custom Ray_t or CTraceFilter handling from
C#. - Incorrect structure layout or invalid vtable resolution will
result in crashes or undefined behavior. - This is considered an
advanced use case intended for engine-level plugins.

------------------------------------------------------------------------

## Notes about ABI & Destructor

-   CRayTraceInterface has a virtual destructor.
-   The object is owned by the Ray-Trace Metamod module.
-   Plugins must never call delete on the interface pointer.
-   C# must only clear its handle on unload.
-   All parameters are passed as native pointers (nint).

# Build

## Requirements

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

GPLv3\
https://www.gnu.org/licenses/gpl-3.0.en.html

## Author

**Michal "Slynx" Přikryl**\
https://slynxdev.cz
