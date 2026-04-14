using System.Numerics;
using System.Runtime.InteropServices;
using CounterStrikeSharp.API;
using CounterStrikeSharp.API.Core;
using CounterStrikeSharp.API.Modules.Entities.Constants;
using CounterStrikeSharp.API.Modules.Memory;
using CounterStrikeSharp.API.Modules.Utils;
using Vector = CounterStrikeSharp.API.Modules.Utils.Vector;

namespace RayTraceAPI
{
#region Native Structs (matching C++ layout exactly)
    [Flags]
    public enum InteractionLayers : ulong
    {
        None = 0,
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

        MASK_SHOT_PHYSICS = Solid | PlayerClip | Window | PassBullets | Player | NPC | Physics_Prop,
        MASK_SHOT_HITBOX = Hitboxes | Player | NPC,
        MASK_SHOT_FULL = MASK_SHOT_PHYSICS | Hitboxes,
        MASK_WORLD_ONLY = Solid | Window | PassBullets,
        MASK_GRENADE = Solid | Window | Physics_Prop | PassBullets,
        MASK_BRUSH_ONLY = Solid | Window,
        MASK_PLAYER_MOVE = Solid | Window | PlayerClip | PassBullets,
        MASK_NPC_MOVE = Solid | Window | NPCClip | PassBullets
    }

    [StructLayout(LayoutKind.Sequential, Pack = 8)]
    public struct TraceOptions
    {
        public ulong InteractsAs;
        public ulong InteractsWith;
        public ulong InteractsExclude;
        public int DrawBeam;

        private int _pad;

        public TraceOptions()
        {
            InteractsAs = 0;
            InteractsWith = (ulong)InteractionLayers.MASK_SHOT_PHYSICS;
            InteractsExclude = 0;
            DrawBeam = 0;
        }

        public TraceOptions(InteractionLayers interactsAs, InteractionLayers interactsWith,
            InteractionLayers interactsExclude = 0, bool drawBeam = false)
        {
            InteractsAs = (ulong)interactsAs;
            InteractsWith = (ulong)interactsWith;
            InteractsExclude = (ulong)interactsExclude;
            DrawBeam = drawBeam ? 1 : 0;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public unsafe struct TraceResult
    {
        public Vector3 StartPos;
        public Vector3 EndPos;
        public Vector3 HitPoint;
        public Vector3 Normal;

        public float Fraction;
        public float HitOffset;

        public int TriangleIndex;
        public short HitboxBoneIndex;
        private short _pad;

        public int Contents;
        public int RayType;

        public bool StartInSolid;
        public bool ExactHitPoint;

        public nint HitEntity;
        public nint Hitbox;
        public nint Surface;
        public nint Body;
        public nint Shape;

        public nint BodyTransform;
        public nint ShapeAttributes;

        public readonly bool DidHit() => Fraction < 1.0f;
        public readonly bool IsAllSolid() => StartInSolid;
        public readonly bool HasExactHit() => ExactHitPoint;

        public readonly int GetTriangleIndex() => TriangleIndex;
        public readonly int GetHitboxBoneIndex() => HitboxBoneIndex;
        public readonly int GetContents() => Contents;
        public readonly int GetRayType() => RayType;

        public readonly Vector3 GetStartPos() => StartPos;
        public readonly Vector3 GetEndPos() => EndPos;
        public readonly Vector3 GetHitPoint() => HitPoint;
        public readonly Vector3 GetNormal() => Normal;

        public readonly float GetFraction() => Fraction;
        public readonly float GetHitOffset() => HitOffset;

        public readonly T* GetPtr<T>(nint ptr) where T : unmanaged
        {
            return (T*)ptr;
        }

        public readonly ref T GetRef<T>(nint ptr) where T : unmanaged
        {
            if (ptr == 0)
                throw new NullReferenceException();

            return ref *(T*)ptr;
        }

        public readonly bool TryGetRef<T>(nint ptr, out T* value) where T : unmanaged
        {
            if (ptr == 0)
            {
                value = null;
                return false;
            }

            value = (T*)ptr;
            return true;
        }

        public readonly CEntityInstance HitEntityPtr() => new(HitEntity);

        public readonly CTransform BodyTransformPtr() => new(BodyTransform);
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct QAngle
    {
        public float X;
        public float Y;
        public float Z;
    }
#endregion

    public interface CRayTraceInterface
    {
        TraceResult TraceShape(
            in Vector3 start,
            in QAngle angles,
            CEntityInstance ignoreEntity,
            in TraceOptions options);

        TraceResult TraceEndShape(
            in Vector3 start,
            in Vector3 end,
            CEntityInstance ignoreEntity,
            in TraceOptions options);

        TraceResult TraceHullShape(
            in Vector3 start,
            in Vector3 end,
            in Vector3 mins,
            in Vector3 maxs,
            CEntityInstance ignoreEntity,
            in TraceOptions options);

        TraceResult TraceShapeEx(
            in Vector3 start,
            in Vector3 end,
            nint traceFilter,
            nint ray);
    }
}