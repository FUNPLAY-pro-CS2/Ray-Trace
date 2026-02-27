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
worlds without duplicating engine detours.

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
        CBaseEntity* pIgnoreEntity,
        const TraceOptions* pOpts,
        TraceResult* pOutResult
    ) = 0;

    virtual bool TraceEndShape(
        const Vector* pOrigin,
        const Vector* pEndOrigin,
        CBaseEntity* pIgnoreEntity,
        const TraceOptions* pOpts,
        TraceResult* pOutResult
    ) = 0;

    virtual bool TraceHullShape(
        const Vector* pVecStart,
        const Vector* pVecEnd,
        const Vector* pHullMins,
        const Vector* pHullMaxs,
        CBaseEntity* pIgnoreEntity,
        const TraceOptions* pOpts,
        TraceResult* pOutResult
    ) = 0;

    virtual bool TraceShapeEx(
        const Vector* pVecStart,
        const Vector* pVecEnd,
        CTraceFilter* pFilterInc,
        Ray_t* rayInc,
        TraceResult* pOutResult
    ) = 0;
};
```

**Return value:**

-   `true` → trace hit something, `TraceResult` is valid\
-   `false` → no hit

------------------------------------------------------------------------

## Getting the interface

### C++ (Metamod plugin)

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

### C# (CounterStrikeSharp plugin)

The recommended way is to use the prebuilt release components:

-   Install **RayTraceImpl** as a CS# plugin:

        addons/counterstrikesharp/plugins/RayTraceImpl/RayTraceImpl.dll

-   Install **RayTraceApi** as a shared assembly:

        addons/counterstrikesharp/shared/RayTraceApi/RayTraceApi.dll

Then reference `RayTraceApi.dll` in your own plugin project and call the
API normally.

------------------------------------------------------------------------

## Calling methods from C++ (Metamod)

### TraceShape example

``` cpp
Vector vecOrigin{};
QAngle angView{};
TraceOptions traceOpts{};
traceOpts.InteractsWith = static_cast<uint64_t>(MASK_SHOT_PHYSICS);
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

------------------------------------------------------------------------

## Calling methods from C# (CounterStrikeSharp plugin)

After installing RayTraceImpl + RayTraceApi from releases:

1.  Add reference in your plugin `.csproj`:

``` xml
<ItemGroup>
  <Reference Include="RayTraceApi">
    <HintPath>libs/RayTraceApi.dll</HintPath>
  </Reference>
</ItemGroup>
```

2.  Use the API:

``` csharp
using RayTraceAPI;
using CounterStrikeSharp.API.Modules.Utils;

internal static PluginCapability<CRayTraceInterface> RayTraceInterface { get; } = new("raytrace:craytraceinterface");

public void DoTrace(CCSPlayerController player)
{
    var rayTrace = RayTraceInterface.Get();
    if (rayTrace == null)
        return;

    var playerPawn = player.PlayerPawn.Value;
    if (playerPawn == null)
        return;

    Vector origin = playerPawn.AbsOrigin!;
    QAngle angles = playerPawn.EyeAngles!;

    TraceOptions options = new();

    if (rayTrace.TraceShape(origin, angles, null, options, out TraceResult result))
    {
        Console.WriteLine($"Hit fraction: {result.Fraction}");
        Console.WriteLine($"EndPos: {result.EndPos}");
    }
}
```

The bridge handles:

-   Interface lookup via MetaFactory
-   Correct vtable offsets (Linux / Windows)
-   Delegate binding
-   Native struct layout alignment

------------------------------------------------------------------------

## VTable offsets (ABI note)

Due to C++ ABI differences:

| Platform            | TraceShape index | TraceEndShape index | TraceHullShape index |
| ------------------- | ---------------- | ------------------- |----------------------|
| Linux (Itanium ABI) | 2                | 3                   | 4                    |
| Windows (MSVC ABI)  | 1                | 2                   | 3                    |

Release builds handle this automatically inside RayTraceImpl.

------------------------------------------------------------------------

## Notes about ABI & Destructor

-   `CRayTraceInterface` has a virtual destructor.
-   The object is owned by the Ray-Trace Metamod module.
-   Plugins must never call `delete` on the interface pointer.
-   C# must only clear its handle on unload.
-   All parameters are passed as native pointers (`nint`).

------------------------------------------------------------------------

## License

GPLv3\
https://www.gnu.org/licenses/gpl-3.0.en.html

------------------------------------------------------------------------

## Author

**Michal "Slynx" Přikryl**\
https://slynxdev.cz
