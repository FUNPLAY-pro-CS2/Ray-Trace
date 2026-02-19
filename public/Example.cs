using System.Runtime.InteropServices;
using CounterStrikeSharp.API;
using CounterStrikeSharp.API.Core;
using CounterStrikeSharp.API.Modules.Memory;
using CounterStrikeSharp.API.Modules.Utils;
using Vector = CounterStrikeSharp.API.Modules.Utils.Vector;
using System.Drawing;
using Vector3 = System.Numerics.Vector3;
using CounterStrikeSharp.API.Modules.Entities.Constants;
using System.Numerics;

namespace RayTrace
{
	[Flags]
	public enum InteractionLayers: ulong
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
	public struct TraceOptions
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

		public TraceOptions(InteractionLayers interactsAs, InteractionLayers interactsWith, InteractionLayers interactsExclude = 0, bool drawBeam = false)
		{
		  InteractsAs = (ulong)interactsAs;
			InteractsWith = (ulong)interactsWith;
			InteractsExclude = (ulong)interactsExclude;
			DrawBeam = drawBeam ? 1 : 0;
		}
	}

	[StructLayout(LayoutKind.Sequential, Size = 8)]
	public struct CUtlString
	{
		public nint _ptr;  // make public so Marshal sees it

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

	[StructLayout(LayoutKind.Sequential)]
	public struct CPhysSurfacePropertiesTrace
	{
		public CUtlString Name;
		public uint NameHash;
		public uint BaseNameHash;
		public int ListIndex;
		public int BaseListIndex;
		[MarshalAs(UnmanagedType.I1)]
		public bool Hidden;
		public CUtlString Description;
		public CPhysSurfacePropertiesPhysicsTrace Physics;
		public CPhysSurfacePropertiesSoundNamesTrace AudioSounds;
		public CPhysSurfacePropertiesAudioTrace AudioParams;
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

	[StructLayout(LayoutKind.Sequential)]
	public struct RnCollisionAttr_t
	{
		public ulong InteractsAs;
		public ulong InteractsWith;
		public ulong InteractsExclude;
		public uint EntityId;
		public uint OwnerId;
		public ushort HierarchyId;
		public CollisionGroup CollisionGroup;
		public CollisionFunctionMask_t CollisionFunctionMask;
	}

	public enum RayType_t : byte
	{
		RAY_TYPE_LINE = 0,
		RAY_TYPE_SPHERE,
		RAY_TYPE_HULL,
		RAY_TYPE_CAPSULE,
		RAY_TYPE_MESH,
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct CPhysSurfacePropertiesPhysicsTrace
	{
		public float Friction;
		public float Elasticity;
		public float Density;
		public float Thickness;
		public float SoftContactFrequency;
		public float SoftContactDampingRatio;
		public float WheelDrag;
		public float HeatConductivity;
		public float Flashpoint;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct CPhysSurfacePropertiesSoundNamesTrace
	{
		public CUtlString ImpactSoft;
		public CUtlString ImpactHard;
		public CUtlString ScrapeSmooth;
		public CUtlString ScrapeRough;
		public CUtlString BulletImpact;
		public CUtlString Rolling;
		public CUtlString Break;
		public CUtlString Strain;
		public CUtlString MeleeImpact;
		public CUtlString PushOff;
		public CUtlString SkidStop;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct CPhysSurfacePropertiesAudioTrace
	{
		public float Reflectivity;
		public float HardnessFactor;
		public float RoughnessFactor;
		public float RoughThreshold;
		public float HardThreshold;
		public float HardVelocityThreshold;
		public float StaticImpactVolume;
		public float OcclusionFactor;
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct CUtlStringToken : IEquatable<CUtlStringToken>
	{
		private uint _hashCode;

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
	

	[StructLayout(LayoutKind.Sequential)]
	public struct CHitBox
	{
		public CUtlString m_name;               // pointer to CUtlString
		public CUtlString m_sSurfaceProperty;   // pointer to CUtlString
		public CUtlString m_sBoneName;          // pointer to CUtlString

		public Vector3 m_vMinBounds;       // blittable
		public Vector3 m_vMaxBounds;       // blittable
		public float m_flShapeRadius;

		public CUtlStringToken m_nBoneNameHash;      // pointer or uint

		public byte m_nShapeType;
		public bool m_bTranslationOnly;
		public uint m_CRC;
		public uint m_cRenderColor;
		public ushort m_nHitBoxIndex;
		public bool m_bForcedTransform;

		public CTransform m_forcedTransform; // only if blittable (no managed types inside)
	}

	[StructLayout(LayoutKind.Sequential, Pack = 16)]
	public struct CTransform
	{
		public Vector4 Position;     // VectorAligned: use Vector4 for 16-byte alignment
		public System.Numerics.Quaternion Orientation;

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
			Position = new Vector4(0, 0, 0, 1);   // vec3_origin + w = 1
			Orientation = System.Numerics.Quaternion.Identity;    // quat_identity
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

    [StructLayout(LayoutKind.Sequential)]
	public struct TraceResult
	{
		// Start position
		public float StartPosX;
		public float StartPosY;
		public float StartPosZ;

		// End position
		public float EndPosX;
		public float EndPosY;
		public float EndPosZ;

		// Hit point
		public float HitPointX;
		public float HitPointY;
		public float HitPointZ;

		// Hit normal
		public float NormalX;
		public float NormalY;
		public float NormalZ;

		// Fraction & hit offset
		public float Fraction;
		public float HitOffset;

		// Hit triangle / hitbox
		public int TriangleIndex;
		public int HitboxBoneIndex;

		// Contents / surface flags (ulong = 8 bytes)
		public InteractionLayers Contents;

		// Ray type (1 byte) aligned automatically by CLR
		public RayType_t RayType;

		// Start in solid & exact hit
		public bool AllSolid;
		public bool ExactHitPoint;

		// Raw pointers (8 bytes each on x64)
		public nint HitEntity;
		public nint Hitbox;
		public nint SurfaceProps;
		public nint BodyHandle;
		public nint ShapeHandle;
		public CTransform BodyTransform;
		public RnCollisionAttr_t ShapeAttributes;

		// Helper properties for vectors
		public Vector3 StartPos => new(StartPosX, StartPosY, StartPosZ);
		public Vector3 EndPos => new(EndPosX, EndPosY, EndPosZ);
		public Vector3 HitPoint => new(HitPointX, HitPointY, HitPointZ);
		public Vector3 Normal => new(NormalX, NormalY, NormalZ);

		public bool DidHit => Fraction < 1.0f;
		public bool IsAllSolid => AllSolid != false;
		public CPhysSurfacePropertiesTrace SurfaceProperties
		{
			get
			{
				if (SurfaceProps == nint.Zero) return default;
				return Marshal.PtrToStructure<CPhysSurfacePropertiesTrace>(SurfaceProps);
			}
		}
		public CHitBox HitBoxTrace
		{
			get
			{
				if (Hitbox == nint.Zero) return default;
				return Marshal.PtrToStructure<CHitBox>(Hitbox);
			}
		}
	}



	public static class CRayTrace
	{
		private static nint g_pRayTraceHandle = nint.Zero;
		private static bool g_bRayTraceLoaded = false;

		private static Func<nint, nint, nint, nint, nint, nint, bool>? _traceShape;
		private static Func<nint, nint, nint, nint, nint, nint, bool>? _traceEndShape;
		private static Func<nint, nint, nint, nint, nint, nint, nint, nint, bool>? _traceHullShape;

		public static void Init()
		{
			g_pRayTraceHandle = (nint)Utilities.MetaFactory("CRayTraceInterface001")!;

			if (g_pRayTraceHandle == nint.Zero)
				throw new Exception("Failed to get Ray-Trace interface handle. Is Ray-Trace MetaMod module loaded?");

			Bind();
			g_bRayTraceLoaded = true;
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

			_traceShape = VirtualFunction.Create<nint, nint, nint, nint, nint, nint, bool>(g_pRayTraceHandle, traceShapeIndex);
			_traceEndShape = VirtualFunction.Create<nint, nint, nint, nint, nint, nint, bool>(g_pRayTraceHandle, traceEndShapeIndex);
			_traceHullShape = VirtualFunction.Create<nint, nint, nint, nint, nint, nint, nint, nint, bool>(g_pRayTraceHandle, traceHullShapeIndex);
		}

		public static unsafe bool TraceShape(Vector origin, QAngle angles, CBaseEntity? ignoreEntity, TraceOptions options, out TraceResult result)
		{
			result = default;

			if (!g_bRayTraceLoaded || g_pRayTraceHandle == nint.Zero)
				return false;

			TraceResult resultBuffer = default;
			TraceOptions optionsBuffer = options;

			bool success = _traceShape!(g_pRayTraceHandle,
										origin.Handle,
										angles.Handle,
										ignoreEntity?.Handle ?? nint.Zero,
										(nint)(&optionsBuffer),
										(nint)(&resultBuffer));

			result = resultBuffer;
			return success;
		}

		public static unsafe bool TraceEndShape(Vector origin, Vector endOrigin, CBaseEntity? ignoreEntity, TraceOptions options, out TraceResult result)
		{
			result = default;

			if (!g_bRayTraceLoaded || g_pRayTraceHandle == nint.Zero)
				return false;

			TraceResult resultBuffer = default;
			TraceOptions optionsBuffer = options;

			bool success = _traceEndShape!(g_pRayTraceHandle,
										   origin.Handle,
										   endOrigin.Handle,
										   ignoreEntity?.Handle ?? nint.Zero,
										   (nint)(&optionsBuffer),
										   (nint)(&resultBuffer));

			result = resultBuffer;
			return success;
		}

    	public static unsafe bool TraceHullShape(Vector vecStart, Vector vecEnd, Vector hullMins, Vector hullMaxs, CBaseEntity? ignoreEntity, TraceOptions options, out TraceResult result)
		{
			result = default;

			if (!g_bRayTraceLoaded || g_pRayTraceHandle == nint.Zero)
				return false;

			TraceResult resultBuffer = default;
			TraceOptions optionsBuffer = options;

			bool success = _traceHullShape!(g_pRayTraceHandle,
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
}
