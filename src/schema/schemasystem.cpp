//
// Created by Michal Přikryl on 26.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "schemasystem.h"
#include "platform.h"
#include "schemasystem/schemasystem.h"
#include "tier1/utlmap.h"
#include <entity2/entityidentity.h>
#include <entity2/entityinstance.h>
#include <edict.h>
#include "CBaseEntity.h"
#include "tier0/memdbgon.h"

#ifdef _WIN32
#define MODULE_PREFIX ""
#define MODULE_EXT ".dll"
#else
#define MODULE_PREFIX "lib"
#define MODULE_EXT ".so"
#endif

namespace
TemplatePlugin
{
    using SchemaKeyValueMap_t = std::map<uint32_t, SchemaKey>;
    using SchemaTableMap_t = std::map<uint32_t, SchemaKeyValueMap_t>;

    static constexpr uint32_t g_ChainKey = hash_32_fnv1a_const("__m_pChainEntity");

    static bool IsFieldNetworked(SchemaClassFieldData_t& field)
    {
        for (int i = 0; i < field.m_nStaticMetadataCount; i++)
        {
            static auto networkEnabled = hash_32_fnv1a_const("MNetworkEnable");
            if (networkEnabled == hash_32_fnv1a_const(field.m_pStaticMetadata[i].m_pszName))
                return true;
        }

        return false;
    }

    // Try to recursively find __m_pChainEntity in base classes
    // (e.g. CCSGameRules -> CTeamplayRules -> CMultiplayRules -> CGameRules, in this case it's in CGameRules)
    static void InitChainOffset(SchemaClassInfoData_t* pClassInfo, SchemaKeyValueMap_t& keyValueMap)
    {
        short fieldsSize = pClassInfo->m_nFieldCount;
        SchemaClassFieldData_t* pFields = pClassInfo->m_pFields;

        for (int i = 0; i < fieldsSize; ++i)
        {
            SchemaClassFieldData_t& field = pFields[i];

            if (hash_32_fnv1a_const(field.m_pszName) != g_ChainKey)
                continue;

            std::pair<uint32_t, SchemaKey> keyValuePair;
            keyValuePair.first = g_ChainKey;
            keyValuePair.second.offset = field.m_nSingleInheritanceOffset;
            keyValuePair.second.networked = IsFieldNetworked(field);

            keyValueMap.insert(keyValuePair);
            return;
        }

        // Not the base class yet, keep looking
        if (pClassInfo->m_nBaseClassCount)
            return InitChainOffset(pClassInfo->m_pBaseClasses[0].m_pClass, keyValueMap);
    }

    static void InitSchemaKeyValueMap(SchemaClassInfoData_t* pClassInfo, SchemaKeyValueMap_t& keyValueMap)
    {
        short fieldsSize = pClassInfo->m_nFieldCount;
        SchemaClassFieldData_t* pFields = pClassInfo->m_pFields;

        for (int i = 0; i < fieldsSize; ++i)
        {
            SchemaClassFieldData_t& field = pFields[i];

            std::pair<uint32_t, SchemaKey> keyValuePair;
            keyValuePair.first = hash_32_fnv1a_const(field.m_pszName);
            keyValuePair.second.offset = field.m_nSingleInheritanceOffset;
            keyValuePair.second.networked = IsFieldNetworked(field);

            keyValueMap.insert(keyValuePair);
        }

        // If this is a child class there might be a parent class with __m_pChainEntity
        if (keyValueMap.find(g_ChainKey) == keyValueMap.end() && pClassInfo->m_nBaseClassCount)
            InitChainOffset(pClassInfo->m_pBaseClasses[0].m_pClass, keyValueMap);
    }

    static bool InitSchemaFieldsForClass(SchemaTableMap_t& tableMap, const char* className, uint32_t classKey)
    {
        CSchemaSystemTypeScope* pType = shared::g_pSchemaSystem->FindTypeScopeForModule(MODULE_PREFIX "server" MODULE_EXT);

        if (!pType)
            return false;

        SchemaClassInfoData_t* pClassInfo = pType->FindDeclaredClass(className).Get();

        if (!pClassInfo)
        {
            SchemaKeyValueMap_t map;
            tableMap.insert(std::make_pair(classKey, map));

            Warning("InitSchemaFieldsForClass(): '%s' was not found!\n", className);
            return false;
        }

        SchemaKeyValueMap_t& keyValueMap = tableMap.insert(std::make_pair(classKey, SchemaKeyValueMap_t())).first->
                                                    second;

        InitSchemaKeyValueMap(pClassInfo, keyValueMap);

        return true;
    }

    int16_t schema::FindChainOffset(const char* className, uint32_t classNameHash)
    {
        return schema::GetOffset(className, classNameHash, "__m_pChainEntity", g_ChainKey).offset;
    }

    int16_t schema::FindChainOffset(const char* className)
    {
        CSchemaSystemTypeScope* pType = shared::g_pSchemaSystem->FindTypeScopeForModule(MODULE_PREFIX "server" MODULE_EXT);

        if (!pType)
            return false;

        SchemaClassInfoData_t* pClassInfo = pType->FindDeclaredClass(className).Get();

        do
        {
            SchemaClassFieldData_t* pFields = pClassInfo->m_pFields;
            short fieldsSize = pClassInfo->m_nFieldCount;
            for (int i = 0; i < fieldsSize; ++i)
            {
                SchemaClassFieldData_t& field = pFields[i];

                if (V_strcmp(field.m_pszName, "__m_pChainEntity") == 0)
                {
                    return field.m_nSingleInheritanceOffset;
                }
            }
        }
        while ((pClassInfo = pClassInfo->m_pBaseClasses ? pClassInfo->m_pBaseClasses->m_pClass : nullptr) != nullptr);

        return 0;
    }

    SchemaKey schema::GetOffset(const char* className, uint32_t classKey, const char* memberName, uint32_t memberKey)
    {
        static SchemaTableMap_t schemaTableMap;

        if (schemaTableMap.find(classKey) == schemaTableMap.end())
        {
            if (InitSchemaFieldsForClass(schemaTableMap, className, classKey))
                return GetOffset(className, classKey, memberName, memberKey);

            return {0, 0};
        }

        SchemaKeyValueMap_t tableMap = schemaTableMap[classKey];

        if (tableMap.find(memberKey) == tableMap.end())
        {
            if (memberKey != g_ChainKey)
                Warning("schema::GetOffset(): '%s' was not found in '%s'!\n", memberName, className);

            return {0, 0};
        }

        return tableMap[memberKey];
    }

    int32_t schema::GetServerOffset(const char* pszClassName, const char* pszPropName)
    {
        SchemaClassInfoData_t* pClassInfo = shared::g_pSchemaSystem->FindTypeScopeForModule(MODULE_PREFIX "server" MODULE_EXT)->
                                                             FindDeclaredClass(pszClassName).Get();
        if (pClassInfo)
        {
            for (int i = 0; i < pClassInfo->m_nFieldCount; i++)
            {
                auto& pFieldData = pClassInfo->m_pFields[i];

                if (std::strcmp(pFieldData.m_pszName, pszPropName) == 0)
                {
                    return pFieldData.m_nSingleInheritanceOffset;
                }
            }
        }

        return -1;
    }

    int32 schema::GetClassSize(const char* className) {
        CSchemaSystemTypeScope *pType = shared::g_pSchemaSystem->FindTypeScopeForModule(
            MODULE_PREFIX "server" MODULE_EXT);

        SchemaClassInfoData_t *pClassInfo = pType->FindDeclaredClass(className).Get();
        if (!pClassInfo) return -1;

        return pClassInfo->m_nSize;
    }

    void NetworkVarStateChanged(uintptr_t pNetworkVar, uint32_t nOffset, uint32 nNetworkStateChangedOffset)
    {
        NetworkStateChangedData data(nOffset);
        CALL_VIRTUAL(void, nNetworkStateChangedOffset, (void*)pNetworkVar, &data);
    }

    void EntityNetworkStateChanged(uintptr_t pEntity, uint nOffset)
    {
        NetworkStateChangedData data(nOffset);
        reinterpret_cast<CEntityInstance*>(pEntity)->NetworkStateChanged(data);
    }

    void ChainNetworkStateChanged(uintptr_t pNetworkVarChainer, uint nLocalOffset)
    {
        CEntityInstance* pEntity = reinterpret_cast<CNetworkVarChainer2*>(pNetworkVarChainer)->m_pEntity;

        if (pEntity)
            // NetworkStateChanged_t WENDER SDK
            // NetworkStateChangedData HL2SDK-CS@
            pEntity->NetworkStateChanged(NetworkStateChangedData(nLocalOffset, -1,
                                                               reinterpret_cast<CNetworkVarChainer2*>(
                                                                   pNetworkVarChainer)->m_PathIndex));
    }
}
