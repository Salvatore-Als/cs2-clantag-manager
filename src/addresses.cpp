#include "abstract.h"
#include "utils/module.h"
#include "addresses.h"

#define RESOLVE_SIG(gameConfig, name, variable) variable = (decltype(variable))gameConfig->ResolveSignature(name)

void CAddresses::Init(CGameConfig *g_CGameConfig)
{
    g_moduleServer = new CModule(GAMEBIN, "server");
	g_moduleEngine = new CModule(ROOTBIN, "engine2");
	g_schemasystem = new CModule(ROOTBIN, "schemasystem");

    RESOLVE_SIG(g_CGameConfig, "NetworkStateChanged", CAddresses::NetworkStateChanged);
    RESOLVE_SIG(g_CGameConfig, "StateChanged", CAddresses::StateChanged);
}

void CAddresses::Clean()
{
    delete g_moduleServer;
	delete g_moduleEngine;
    delete g_schemasystem;
} 