//
// Created by Michal Přikryl on 20.06.2025.
// Copyright (c) 2025 slynxcz. All rights reserved.
//
#include "gameconfig.h"
#include <fstream>
#include <Plugin.h>

namespace RayTracePlugin
{
    CGameConfig::CGameConfig(const std::string& path) { m_sPath = path; }

    CGameConfig::~CGameConfig() = default;

    bool CGameConfig::Init(char* conf_error, int conf_error_size)
    {
        std::ifstream ifs(m_sPath);
        if (!ifs)
        {
            V_snprintf(conf_error, conf_error_size, "Gamedata file not found.");
            return false;
        }

        m_json = json::parse(ifs);

#if _WIN32
    constexpr auto platform = "windows";
#else
        constexpr auto platform = "linux";
#endif

        try
        {
            for (auto& [k, v] : m_json.items())
            {
                if (v.contains("signatures"))
                {
                    if (auto library = v["signatures"]["library"]; library.is_string())
                    {
                        m_umLibraries[k] = library.get<std::string>();
                    }
                    if (auto signature = v["signatures"][platform]; signature.is_string())
                    {
                        m_umSignatures[k] = signature.get<std::string>();
                    }
                }
                if (v.contains("offsets"))
                {
                    if (auto offset = v["offsets"][platform]; offset.is_number_integer())
                    {
                        m_umOffsets[k] = offset.get<std::int64_t>();
                    }
                }
                if (v.contains("patches"))
                {
                    if (auto patch = v["patches"][platform]; patch.is_string())
                    {
                        m_umPatches[k] = patch.get<std::string>();
                    }
                }
            }
        }
        catch (const std::exception& ex)
        {
            V_snprintf(conf_error, conf_error_size, "Failed to parse gamedata file: %s", ex.what());
            return false;
        }
        return true;
    }

    const std::string CGameConfig::GetPath() { return m_sPath; }

    const char* CGameConfig::GetSignature(const std::string& name)
    {
        auto it = m_umSignatures.find(name);
        if (it == m_umSignatures.end())
        {
            return nullptr;
        }
        return it->second.c_str();
    }

    const char* CGameConfig::GetPatch(const std::string& name)
    {
        auto it = m_umPatches.find(name);
        if (it == m_umPatches.end())
        {
            return nullptr;
        }
        return it->second.c_str();
    }

    int CGameConfig::GetOffset(const std::string& name)
    {
        auto it = m_umOffsets.find(name);
        if (it == m_umOffsets.end())
        {
            return -1;
        }
        return it->second;
    }
}
