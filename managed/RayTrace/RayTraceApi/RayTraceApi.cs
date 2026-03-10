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

    [StructLayout(LayoutKind.Explicit, Size = 32)]
    public unsafe struct TraceOptions
    {
        [FieldOffset(0)] public ulong InteractsAs;
        [FieldOffset(8)] public ulong InteractsWith;
        [FieldOffset(16)] public ulong InteractsExclude;
        [FieldOffset(24)] public int DrawBeam;

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

    [StructLayout(LayoutKind.Explicit, Size = 8)]
    public unsafe struct CUtlString
    {
        [FieldOffset(0)] public nint _ptr; // make public so Marshal sees it

        public string Value
        {
            get
            {
                if (_ptr == 0 || _ptr == IntPtr.MaxValue) return string.Empty;
                return Marshal.PtrToStringUTF8(_ptr)!; // read UTF8 string from native pointer
            }
        }

        public static implicit operator string(CUtlString s) => s.Value;
    }

    [StructLayout(LayoutKind.Explicit, Size = 200)]
    public unsafe struct CPhysSurfacePropertiesTrace
    {
        [FieldOffset(0)] public CUtlString Name;
        [FieldOffset(8)] public uint NameHash;
        [FieldOffset(12)] public uint BaseNameHash;
        [FieldOffset(16)] public int ListIndex;
        [FieldOffset(20)] public int BaseListIndex;

        [MarshalAs(UnmanagedType.I1)] [FieldOffset(24)]
        public bool Hidden;

        [FieldOffset(32)] public CUtlString Description;
        [FieldOffset(40)] public CPhysSurfacePropertiesPhysicsTrace Physics;
        [FieldOffset(80)] public CPhysSurfacePropertiesSoundNamesTrace AudioSounds;
        [FieldOffset(168)] public CPhysSurfacePropertiesAudioTrace AudioParams;
    }

    public enum CollisionFunctionMask_t : byte
    {
        FCOLLISION_FUNC_ENABLE_SOLID_CONTACT = (1 << 0),
        FCOLLISION_FUNC_ENABLE_TRACE_QUERY = (1 << 1),
        FCOLLISION_FUNC_ENABLE_TOUCH_EVENT = (1 << 2),
        FCOLLISION_FUNC_ENABLE_SELF_COLLISIONS = (1 << 3),
        FCOLLISION_FUNC_IGNORE_FOR_HITBOX_TEST = (1 << 4),
        FCOLLISION_FUNC_ENABLE_TOUCH_PERSISTS = (1 << 5),
    }

    [StructLayout(LayoutKind.Explicit, Size = 48)]
    public unsafe struct RnCollisionAttr_t
    {
        [FieldOffset(0)] public ulong InteractsAs;
        [FieldOffset(8)] public ulong InteractsWith;
        [FieldOffset(16)] public ulong InteractsExclude;
        [FieldOffset(24)] public uint EntityId;
        [FieldOffset(28)] public uint OwnerId;
        [FieldOffset(32)] public ushort HierarchyId;
        [FieldOffset(36)] public CollisionGroup CollisionGroup;
        [FieldOffset(40)] public CollisionFunctionMask_t CollisionFunctionMask;
    }

    public enum RayType_t : byte
    {
        RAY_TYPE_LINE = 0,
        RAY_TYPE_SPHERE,
        RAY_TYPE_HULL,
        RAY_TYPE_CAPSULE,
        RAY_TYPE_MESH,
    }

    [StructLayout(LayoutKind.Explicit, Size = 36)]
    public unsafe struct CPhysSurfacePropertiesPhysicsTrace
    {
        [FieldOffset(0)] public float Friction;
        [FieldOffset(4)] public float Elasticity;
        [FieldOffset(8)] public float Density;
        [FieldOffset(12)] public float Thickness;
        [FieldOffset(16)] public float SoftContactFrequency;
        [FieldOffset(20)] public float SoftContactDampingRatio;
        [FieldOffset(24)] public float WheelDrag;
        [FieldOffset(28)] public float HeatConductivity;
        [FieldOffset(32)] public float Flashpoint;
    }

    [StructLayout(LayoutKind.Explicit, Size = 88)]
    public unsafe struct CPhysSurfacePropertiesSoundNamesTrace
    {
        [FieldOffset(0)] public CUtlString ImpactSoft;
        [FieldOffset(8)] public CUtlString ImpactHard;
        [FieldOffset(16)] public CUtlString ScrapeSmooth;
        [FieldOffset(24)] public CUtlString ScrapeRough;
        [FieldOffset(32)] public CUtlString BulletImpact;
        [FieldOffset(40)] public CUtlString Rolling;
        [FieldOffset(48)] public CUtlString Break;
        [FieldOffset(56)] public CUtlString Strain;
        [FieldOffset(64)] public CUtlString MeleeImpact;
        [FieldOffset(72)] public CUtlString PushOff;
        [FieldOffset(80)] public CUtlString SkidStop;
    }

    [StructLayout(LayoutKind.Explicit, Size = 32)]
    public unsafe struct CPhysSurfacePropertiesAudioTrace
    {
        [FieldOffset(0)] public float Reflectivity;
        [FieldOffset(4)] public float HardnessFactor;
        [FieldOffset(8)] public float RoughnessFactor;
        [FieldOffset(12)] public float RoughThreshold;
        [FieldOffset(16)] public float HardThreshold;
        [FieldOffset(20)] public float HardVelocityThreshold;
        [FieldOffset(24)] public float StaticImpactVolume;
        [FieldOffset(28)] public float OcclusionFactor;
    }

    [StructLayout(LayoutKind.Explicit, Size = 4)]
    public unsafe struct CUtlStringToken : IEquatable<CUtlStringToken>
    {
        [FieldOffset(0)] private uint _hashCode;

        public CUtlStringToken(uint hashCode)
        {
            _hashCode = hashCode;
        }

        public bool IsValid => _hashCode != 0;

        public uint GetHashCodeValue() => _hashCode;

        public void SetHashCode(uint hash) => _hashCode = hash;

        public bool Equals(CUtlStringToken other)
            => _hashCode == other._hashCode;

        public override bool Equals(object? obj)
            => obj is CUtlStringToken other && Equals(other);

        public override int GetHashCode()
            => (int)_hashCode;

        public static bool operator ==(CUtlStringToken a, CUtlStringToken b)
            => a._hashCode == b._hashCode;

        public static bool operator !=(CUtlStringToken a, CUtlStringToken b)
            => a._hashCode != b._hashCode;

        public static bool operator <(CUtlStringToken a, CUtlStringToken b)
            => a._hashCode < b._hashCode;

        public static bool operator >(CUtlStringToken a, CUtlStringToken b)
            => a._hashCode > b._hashCode;

        public override string ToString()
            => $"0x{_hashCode:X8}";
    }


    [StructLayout(LayoutKind.Explicit, Size = 104)]
    public unsafe struct CHitBox
    {
        [FieldOffset(0)] public CUtlString m_name; // pointer to CUtlString
        [FieldOffset(8)] public CUtlString m_sSurfaceProperty; // pointer to CUtlString
        [FieldOffset(16)] public CUtlString m_sBoneName; // pointer to CUtlString

        [FieldOffset(24)] public Vector3 m_vMinBounds; // blittable
        [FieldOffset(36)] public Vector3 m_vMaxBounds; // blittable
        [FieldOffset(48)] public float m_flShapeRadius;

        [FieldOffset(52)] public CUtlStringToken m_nBoneNameHash; // pointer or uint

        [FieldOffset(56)] public byte m_nShapeType;
        [FieldOffset(57)] public bool m_bTranslationOnly;
        [FieldOffset(60)] public uint m_CRC;
        [FieldOffset(64)] public uint m_cRenderColor;
        [FieldOffset(68)] public ushort m_nHitBoxIndex;
        [FieldOffset(70)] public bool m_bForcedTransform;

        [FieldOffset(72)] public CTransform m_forcedTransform; // only if blittable (no managed types inside)
    }

    [StructLayout(LayoutKind.Explicit, Pack = 16, Size = 32)]
    public unsafe struct CTransform
    {
        [FieldOffset(0)] public Vector4 Position; // VectorAligned: use Vector4 for 16-byte alignment
        [FieldOffset(16)] public System.Numerics.Quaternion Orientation;

        public CTransform(Vector3 position, System.Numerics.Quaternion orientation)
        {
            Position = new Vector4(position, 1.0f); // w = 1.0f like vec3_origin.w
            Orientation = orientation;
        }

        public bool IsValid()
        {
            return !Position.Equals(Vector4.Zero) && Orientation != System.Numerics.Quaternion.Identity;
        }

        public void SetToIdentity()
        {
            Position = new Vector4(0, 0, 0, 1); // vec3_origin + w = 1
            Orientation = System.Numerics.Quaternion.Identity; // quat_identity
        }

        public static bool operator ==(CTransform a, CTransform b)
        {
            return a.Position == b.Position && a.Orientation == b.Orientation;
        }

        public static bool operator !=(CTransform a, CTransform b)
        {
            return !(a == b);
        }

        public override bool Equals(object? obj)
        {
            return obj is CTransform t && this == t;
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(Position, Orientation);
        }

        public override string ToString()
        {
            return $"CTransform(Position: {Position}, Orientation: {Orientation})";
        }
    }

    [StructLayout(LayoutKind.Explicit, Size = 208)]
    public struct TraceResult
    {
        [FieldOffset(0)] public float StartPosX;
        [FieldOffset(4)] public float StartPosY;
        [FieldOffset(8)] public float StartPosZ;
        [FieldOffset(12)] public float EndPosX;
        [FieldOffset(16)] public float EndPosY;
        [FieldOffset(20)] public float EndPosZ;
        [FieldOffset(24)] public float HitPointX;
        [FieldOffset(28)] public float HitPointY;
        [FieldOffset(32)] public float HitPointZ;
        [FieldOffset(36)] public float NormalX;
        [FieldOffset(40)] public float NormalY;
        [FieldOffset(44)] public float NormalZ;
        [FieldOffset(48)] public float Fraction;
        [FieldOffset(52)] public float HitOffset;

        [FieldOffset(56)] public int TriangleIndex;
        [FieldOffset(60)] public int HitboxBoneIndex;
        [FieldOffset(64)] public InteractionLayers Contents;
        [FieldOffset(68)] public RayType_t RayType;
        [FieldOffset(72)] public int AllSolid;
        [FieldOffset(76)] public int ExactHitPoint;

        [FieldOffset(80)] public nint HitEntityHandle;
        [FieldOffset(88)] public nint HitboxHandle;
        [FieldOffset(96)] public nint SurfacePropsHandle;
        [FieldOffset(104)] public nint BodyHandle;
        [FieldOffset(112)] public nint ShapeHandle;

        [FieldOffset(128)] public CTransform BodyTransform;
        [FieldOffset(160)] public RnCollisionAttr_t ShapeAttributes;

        public Vector3 StartPos => new(StartPosX, StartPosY, StartPosZ);
        public Vector3 EndPos => new(EndPosX, EndPosY, EndPosZ);
        public Vector3 HitPoint => new(HitPointX, HitPointY, HitPointZ);
        public Vector3 Normal => new(NormalX, NormalY, NormalZ);

        public bool DidHit => Fraction < 1.0f;
        public bool IsAllSolid => AllSolid != 0;
        public bool HasExactHit => ExactHitPoint != 0;

        public CEntityInstance? HitEntity
            => HitEntityHandle == 0 ? null : new CEntityInstance(HitEntityHandle);

        public CHitBox Hitbox
            => HitboxHandle == 0 ? default : Marshal.PtrToStructure<CHitBox>(HitboxHandle);

        public CPhysSurfacePropertiesTrace SurfaceProps
            => SurfacePropsHandle == 0
                ? default
                : Marshal.PtrToStructure<CPhysSurfacePropertiesTrace>(SurfacePropsHandle);
    }
#endregion

    public interface CRayTraceInterface
    {
        public unsafe bool TraceShape(Vector origin, QAngle angles, CBaseEntity? ignoreEntity, TraceOptions options,
            out TraceResult result);

        public unsafe bool TraceEndShape(Vector origin, Vector endOrigin, CBaseEntity? ignoreEntity,
            TraceOptions options, out TraceResult result);

        public unsafe bool TraceHullShape(Vector vecStart, Vector vecEnd, Vector hullMins, Vector hullMaxs,
            CBaseEntity? ignoreEntity, TraceOptions options, out TraceResult result);
    }
}
