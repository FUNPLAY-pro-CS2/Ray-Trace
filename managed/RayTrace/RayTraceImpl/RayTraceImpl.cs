//
// Created by Michal Přikryl on 10.10.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
using System.Buffers;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Loader;
using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;
using CounterStrikeSharp.API;
using CounterStrikeSharp.API.Core;
using CounterStrikeSharp.API.Core.Attributes.Registration;
using CounterStrikeSharp.API.Core.Capabilities;
using CounterStrikeSharp.API.Modules.Commands;
using CounterStrikeSharp.API.Modules.Memory;
using CounterStrikeSharp.API.Modules.Utils;
using RayTraceAPI;

namespace RayTraceImpl;

// ReSharper disable once InconsistentNaming
// ReSharper disable once UnusedType.Global
public class RayTraceImpl : BasePlugin
{
    public override string ModuleName => "RayTraceImpl";
    public override string ModuleVersion => "v1.0.0";
    public override string ModuleAuthor => "Slynx";

    internal static PluginCapability<CRayTraceInterface> RayTraceInterface { get; } = new("raytrace:craytraceinterface");

    public override void Load(bool hotReload)
    {
        Capabilities.RegisterPluginCapability(RayTraceInterface, () => new CRayTrace());
        RegisterListener<Listeners.OnMetamodAllPluginsLoaded>(OnMetamodAllPluginsLoaded);
    }

    public override void Unload(bool hotReload)
    {
        RemoveListener<Listeners.OnMetamodAllPluginsLoaded>(OnMetamodAllPluginsLoaded);
    }

    private void OnMetamodAllPluginsLoaded()
    {
        if (!NativeBridge.Initialize())
        {
            Prints.ServerLog("[RayTraceImpl] Native bridge initialization failed.", ConsoleColor.Red);
            return;
        }
        Prints.ServerLog("[RayTraceImpl] Managed side initialized.", ConsoleColor.Green);
    }
}

public class CRayTrace : CRayTraceInterface
{
    public unsafe bool TraceShape(Vector origin, QAngle angles, CBaseEntity? ignoreEntity, TraceOptions options, out TraceResult result)
    {
        result = default;

        if (!NativeBridge.m_bRayTraceLoaded || NativeBridge.m_pRayTraceHandle == nint.Zero)
            return false;

        TraceResult resultBuffer = default;
        TraceOptions optionsBuffer = options;

        bool success = NativeBridge._traceShape!(NativeBridge.m_pRayTraceHandle,
            origin.Handle,
            angles.Handle,
            ignoreEntity?.Handle ?? nint.Zero,
            (nint)(&optionsBuffer),
            (nint)(&resultBuffer));

        result = resultBuffer;
        return success;
    }

    public unsafe bool TraceEndShape(Vector origin, Vector endOrigin, CBaseEntity? ignoreEntity, TraceOptions options, out TraceResult result)
    {
        result = default;

        if (!NativeBridge.m_bRayTraceLoaded || NativeBridge.m_pRayTraceHandle == nint.Zero)
            return false;

        TraceResult resultBuffer = default;
        TraceOptions optionsBuffer = options;

        bool success = NativeBridge._traceEndShape!(NativeBridge.m_pRayTraceHandle,
            origin.Handle,
            endOrigin.Handle,
            ignoreEntity?.Handle ?? nint.Zero,
            (nint)(&optionsBuffer),
            (nint)(&resultBuffer));

        result = resultBuffer;
        return success;
    }

    public unsafe bool TraceHullShape(Vector vecStart, Vector vecEnd, Vector hullMins, Vector hullMaxs, CBaseEntity? ignoreEntity, TraceOptions options, out TraceResult result)
    {
        result = default;

        if (!NativeBridge.m_bRayTraceLoaded || NativeBridge.m_pRayTraceHandle == nint.Zero)
            return false;

        TraceResult resultBuffer = default;
        TraceOptions optionsBuffer = options;

        bool success = NativeBridge._traceHullShape!(NativeBridge.m_pRayTraceHandle,
            vecStart.Handle,
            vecEnd.Handle,
            hullMins.Handle,
            hullMaxs.Handle,
            ignoreEntity?.Handle ?? nint.Zero,
            (nint)(&optionsBuffer),
            (nint)(&resultBuffer));

        result = resultBuffer;
        return success;
    }
}

public static class NativeBridge
{
    public static nint m_pRayTraceHandle = nint.Zero;
    public static bool m_bRayTraceLoaded = false;

    public static Func<nint, nint, nint, nint, nint, nint, bool>? _traceShape;
    public static Func<nint, nint, nint, nint, nint, nint, bool>? _traceEndShape;
    public static Func<nint, nint, nint, nint, nint, nint, nint, nint, bool>? _traceHullShape;

    public static bool Initialize()
    {
        m_pRayTraceHandle = (nint)Utilities.MetaFactory("CRayTraceInterface001")!;

        if (m_pRayTraceHandle == nint.Zero)
        {
            Prints.ServerLog("[RayTraceImpl] Failed to get Ray-Trace interface handle. Is Ray-Trace MetaMod module loaded?", ConsoleColor.Red);
            return false;
        }

        Bind();
        m_bRayTraceLoaded = true;
        return true;
    }

    private static void Bind()
    {
        int traceShapeIndex = 2;
        int traceEndShapeIndex = 3;
        int traceHullShapeIndex = 4;

        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            traceShapeIndex = 1;
            traceEndShapeIndex = 2;
            traceHullShapeIndex = 3;
        }

        _traceShape = VirtualFunction.Create<nint, nint, nint, nint, nint, nint, bool>(m_pRayTraceHandle, traceShapeIndex);
        _traceEndShape = VirtualFunction.Create<nint, nint, nint, nint, nint, nint, bool>(m_pRayTraceHandle, traceEndShapeIndex);
        _traceHullShape = VirtualFunction.Create<nint, nint, nint, nint, nint, nint, nint, nint, bool>(m_pRayTraceHandle, traceHullShapeIndex);
    }
}

public static class Prints
{
    public static void ServerLog(string msg, ConsoleColor color = ConsoleColor.White)
    {
        Console.ForegroundColor = color;
        Console.WriteLine(msg);
        Console.ResetColor();
    }
}