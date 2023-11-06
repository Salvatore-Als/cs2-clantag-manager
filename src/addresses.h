#pragma once

#include "abstract.h"
#include "utils/module.h"
#include "gameconfig.h"

class CBasePlayerController;
class CEntityInstance;
class CGameConfig;

extern CModule *g_moduleServer;

class CAddresses
{
public:
    CAddresses()
    {

    }

    void Init(CGameConfig *g_CGameConfig);  // using it b'cause of cirtular dependencies

    CModule *GetModuleServer() { return g_moduleServer; }
    CModule *GetModuleEngine() { return g_moduleEngine; }
    CModule *GetShemaSystem() { return g_schemasystem; }

    void (*NetworkStateChanged)(int64 chainEntity, int64 offset, int64 a3);
    void (*StateChanged)(void *networkTransmitComponent, CEntityInstance *ent, int64 offset, int16 a4, int16 a5);

    void Clean();

private:
    CModule *g_moduleServer;
    CModule *g_schemasystem;
    CModule *g_moduleEngine;
};