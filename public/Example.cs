using System.Numerics;
using System.Runtime.InteropServices;
using CounterStrikeSharp.API;
using CounterStrikeSharp.API.Core;
using CounterStrikeSharp.API.Modules.Memory;
using CounterStrikeSharp.API.Modules.Utils;
using Vector = CounterStrikeSharp.API.Modules.Utils.Vector;

namespace RayTrace
{
#region Native Structs (matching C++ layout exactly)
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

	[StructLayout(LayoutKind.Explicit, Size = 44)]
	public struct TraceResult
	{
		[FieldOffset(0)] public float EndPosX;
		[FieldOffset(4)] public float EndPosY;
		[FieldOffset(8)] public float EndPosZ;
		[FieldOffset(16)] public nint HitEntity;
		[FieldOffset(24)] public float Fraction;
		[FieldOffset(28)] public int AllSolid;
		[FieldOffset(32)] public float NormalX;
		[FieldOffset(36)] public float NormalY;
		[FieldOffset(40)] public float NormalZ;

		public Vector3 EndPos => new(EndPosX, EndPosY, EndPosZ);
		public Vector3 Normal => new(NormalX, NormalY, NormalZ);
		public bool DidHit => Fraction < 1.0f;
		public bool IsAllSolid => AllSolid != 0;
	}
#endregion

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
