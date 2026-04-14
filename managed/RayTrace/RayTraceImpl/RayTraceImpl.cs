//
// Created by Michal Přikryl on 10.10.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
using System.Buffers;
using System.Numerics;
using System.Reflection;
using System.Runtime.CompilerServices;
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
using QAngle = RayTraceAPI.QAngle;
using Vector = CounterStrikeSharp.API.Modules.Utils.Vector;

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
    public TraceResult TraceShape(in Vector3 start, in QAngle angles, CEntityInstance ignore, in TraceOptions options)
    {
        unsafe
        {
            if (!NativeBridge.Loaded)
                return default;

            return NativeBridge.TraceShape!(
                NativeBridge.Handle,
                start,
                angles,
                ignore.Handle,
                (nint)Unsafe.AsPointer(ref Unsafe.AsRef(in options))
            );
        }
    }

    public TraceResult TraceEndShape(in Vector3 start, in Vector3 end, CEntityInstance ignore, in TraceOptions options)
    {
        unsafe
        {
            if (!NativeBridge.Loaded)
                return default;

            return NativeBridge.TraceEndShape!(
                NativeBridge.Handle,
                start,
                end,
                ignore.Handle,
                (nint)Unsafe.AsPointer(ref Unsafe.AsRef(in options))
            );
        }
    }

    public TraceResult TraceHullShape(in Vector3 start, in Vector3 end, in Vector3 mins, in Vector3 maxs, CEntityInstance ignore, in TraceOptions options)
    {
        unsafe
        {
            if (!NativeBridge.Loaded)
                return default;

            return NativeBridge.TraceHullShape!(
                NativeBridge.Handle,
                start,
                end,
                mins,
                maxs,
                ignore.Handle,
                (nint)Unsafe.AsPointer(ref Unsafe.AsRef(in options))
            );
        }
    }

    public TraceResult TraceShapeEx(in Vector3 start, in Vector3 end, nint filter, nint ray)
    {
        unsafe
        {
            if (!NativeBridge.Loaded)
                return default;

            return NativeBridge.TraceShapeEx!(
                NativeBridge.Handle,
                start,
                end,
                filter,
                ray
            );
        }
    }
}

public static class NativeBridge
{
    public static nint Handle;
    public static bool Loaded;

    public static Func<nint, Vector3, QAngle, nint, nint, TraceResult>? TraceShape;
    public static Func<nint, Vector3, Vector3, nint, nint, TraceResult>? TraceEndShape;
    public static Func<nint, Vector3, Vector3, Vector3, Vector3, nint, nint, TraceResult>? TraceHullShape;
    public static Func<nint, Vector3, Vector3, nint, nint, TraceResult>? TraceShapeEx;

    public static bool Initialize()
    {
        Handle = (nint)Utilities.MetaFactory("CRayTraceInterface001")!;

        if (Handle == 0)
            return false;

        Bind();
        Loaded = true;
        return true;
    }

    private static void Bind()
    {
        int shape, end, hull, ex;

        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
        {
            shape = 1;
            end = 2;
            hull = 3;
            ex = 4;
        }
        else
        {
            shape = 2;
            end = 3;
            hull = 4;
            ex = 5;
        }

        TraceShape = VirtualFunction.Create<nint, Vector3, QAngle, nint, nint, TraceResult>(Handle, shape);
        TraceEndShape = VirtualFunction.Create<nint, Vector3, Vector3, nint, nint, TraceResult>(Handle, end);
        TraceHullShape = VirtualFunction.Create<nint, Vector3, Vector3, Vector3, Vector3, nint, nint, TraceResult>(Handle, hull);
        TraceShapeEx = VirtualFunction.Create<nint, Vector3, Vector3, nint, nint, TraceResult>(Handle, ex);
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