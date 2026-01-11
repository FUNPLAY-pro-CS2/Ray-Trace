#ifndef _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_
#define _INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_

#include <igameevents.h>
#include <ISmmPlugin.h>
#include "entitysystem.h"

namespace TemplatePlugin
{
    class ITemplatePlugin : public ISmmPlugin, public IMetamodListener
    {
    public:
        bool Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late) override;
        bool Unload(char *error, size_t maxlen) override;
        const char *GetAuthor() override;
        const char *GetName() override;
        const char *GetDescription() override;
        const char *GetURL() override;
        const char *GetLicense() override;
        const char *GetVersion() override;
        const char *GetDate() override;
        const char *GetLogTag() override;
    };

    extern ITemplatePlugin g_iPlugin;
}

#endif //_INCLUDE_METAMOD_SOURCE_STUB_PLUGIN_H_

PLUGIN_GLOBALVARS();
