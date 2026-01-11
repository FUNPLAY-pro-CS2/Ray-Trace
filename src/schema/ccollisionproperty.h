//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#pragma once

#include "CBaseEntity.h"

namespace RayTracePlugin {
    struct VPhysicsCollisionAttribute_t
    {
        DECLARE_SCHEMA_CLASS_INLINE(VPhysicsCollisionAttribute_t)

        SCHEMA_FIELD(uint8, m_nCollisionGroup)
        SCHEMA_FIELD(uint64_t, m_nInteractsAs)
        SCHEMA_FIELD(uint64_t, m_nInteractsWith)
        SCHEMA_FIELD(uint64_t, m_nInteractsExclude)
	    SCHEMA_FIELD(uint16, m_nHierarchyId)
    };

    class CCollisionProperty
    {
    public:
        DECLARE_SCHEMA_CLASS_INLINE(CCollisionProperty)

        SCHEMA_FIELD(VPhysicsCollisionAttribute_t, m_collisionAttribute)
        SCHEMA_FIELD(SolidType_t, m_nSolidType)
        SCHEMA_FIELD(uint8, m_usSolidFlags)
        SCHEMA_FIELD(uint8, m_CollisionGroup)
        SCHEMA_FIELD(Vector, m_vecMins)
        SCHEMA_FIELD(Vector, m_vecMaxs)
    };
}