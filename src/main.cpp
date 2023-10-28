#include <stdio.h>
#include "main.h"
#include "gameconfig.h"
#include <string>
#include "abstract.h"
#include "addresses.h"
#include "utils/module.h"
#include "entity2/entitysystem.h"
#include "entity/cbaseentity.h"
#include <iostream>
#include <cstdlib>
#include "detours.h"
#include "cs2_sdk/interfaces/cgameresourceserviceserver.h"
#include "filemanager.h"

void Debug(const char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	char buf[1024] = {};
	V_vsnprintf(buf, sizeof(buf) - 1, msg, args);

	ConColorMsg(Color(255, 0, 255, 255), "[%s] %s\n", PREFIX, buf);

	va_end(args);
}

void Fatal(const char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	char buf[1024] = {};
	V_vsnprintf(buf, sizeof(buf) - 1, msg, args);

	ConColorMsg(Color(255, 0, 0, 255), "[%s] %s\n", PREFIX, buf);

	va_end(args);
}

void Warn(const char *msg, ...)
{
	va_list args;
	va_start(args, msg);

	char buf[1024] = {};
	V_vsnprintf(buf, sizeof(buf) - 1, msg, args);

	ConColorMsg(Color(255, 165, 0, 255), "[%s] %s\n", PREFIX, buf);

	va_end(args);
}

class GameSessionConfiguration_t
{
};

SH_DECL_HOOK3_void(INetworkServerService, StartupServer, SH_NOATTRIB, 0, const GameSessionConfiguration_t &, ISource2WorldSession *, const char *);
SH_DECL_HOOK4_void(IServerGameClients, ClientPutInServer, SH_NOATTRIB, 0, CPlayerSlot, char const *, int, uint64);

CPlugin g_CPlugin;
IVEngineServer2 *g_pEngine = nullptr;
ICvar *icvar = nullptr;
IGameEventManager2 *g_pGameEventManager = nullptr;

CGlobalVars *g_pGlobals = nullptr;
CAddresses *g_CAddresses = nullptr;
CFileManager *g_CFileManager = nullptr;

CEntitySystem *g_pEntitySystem = nullptr;
CGameResourceService *g_gGameResourceServiceServe = nullptr;
CGameConfig *g_CGameConfig;
CSchemaSystem *g_pSchemaSystem2 = nullptr;

CGlobalVars *GetServerGlobals()
{
	return g_pEngine->GetServerGlobals();
}

PLUGIN_EXPOSE(CPlugin, g_CPlugin);
bool CPlugin::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();

	GET_V_IFACE_ANY(GetEngineFactory, g_pNetworkServerService, INetworkServerService, NETWORKSERVERSERVICE_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, g_pEngine, IVEngineServer2, SOURCE2ENGINETOSERVER_INTERFACE_VERSION);
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
	GET_V_IFACE_ANY(GetFileSystemFactory, g_pFullFileSystem, IFileSystem, FILESYSTEM_INTERFACE_VERSION);
	GET_V_IFACE_ANY(GetServerFactory, g_pSource2GameClients, IServerGameClients, SOURCE2GAMECLIENTS_INTERFACE_VERSION);
	GET_V_IFACE_ANY(GetServerFactory, g_pSource2Server, ISource2Server, SOURCE2SERVER_INTERFACE_VERSION);

	SH_ADD_HOOK_MEMFUNC(INetworkServerService, StartupServer, g_pNetworkServerService, this, &CPlugin::Hook_StartupServer, true);
	SH_ADD_HOOK_MEMFUNC(IServerGameClients, ClientPutInServer, g_pSource2GameClients, this, &CPlugin::Hook_ClientPutInServer, true);

	g_CFileManager = new CFileManager();
	if (!g_CFileManager->Init(g_pFullFileSystem))
	{
		Fatal("Unable to int file manager");
		return false;
	}

	g_CGameConfig = new CGameConfig();
	char szGameConfigError[255] = "";
	if (!g_CGameConfig->Init(g_pFullFileSystem, szGameConfigError, sizeof(szGameConfigError)))
	{
		snprintf(error, maxlen, "Could not read %s: %s", g_CGameConfig->GetPath().c_str(), szGameConfigError);
		Fatal("%s", error);
		return false;
	}

	g_pGlobals = GetServerGlobals();

	g_CAddresses = new CAddresses();
	g_CAddresses->Init(g_CGameConfig);

	CModule *pSchemaSystem = g_CAddresses->GetShemaSystem();
	g_pSchemaSystem2 = (CSchemaSystem *)pSchemaSystem->FindInterface(SCHEMASYSTEM_INTERFACE_VERSION);
	if (!g_pSchemaSystem2)
	{
		Error("Unable to find CSchemaSystem interface");
		return false;
	}

	CModule *pModuleEngine = g_CAddresses->GetModuleEngine();
	g_gGameResourceServiceServe = (CGameResourceService *)pModuleEngine->FindInterface(GAMERESOURCESERVICESERVER_INTERFACE_VERSION);
	if (!g_gGameResourceServiceServe)
	{
		Fatal("Unable to find the CGameResourceService interface");
		return false;
	}

	g_pGameEventManager = (IGameEventManager2 *)(CALL_VIRTUAL(uintptr_t, 91, g_pSource2Server) - 8);
	if (!g_pGameEventManager)
	{
		Fatal("Unable to find the IGameEventManager2");
		return false;
	}

	if (!InitDetours(g_CGameConfig))
	{
		Fatal("Unable to init Detours");
		return false;
	}

	g_pEntitySystem = g_gGameResourceServiceServe->GetGameEntitySystem();

	g_pCVar = icvar;
	ConVar_Register(FCVAR_RELEASE | FCVAR_CLIENT_CAN_EXECUTE | FCVAR_GAMEDLL);

	return true;
}

bool CPlugin::Unload(char *error, size_t maxlen)
{
	SH_REMOVE_HOOK_MEMFUNC(INetworkServerService, StartupServer, g_pNetworkServerService, this, &CPlugin::Hook_StartupServer, true);
	SH_REMOVE_HOOK_MEMFUNC(IServerGameClients, ClientPutInServer, g_pSource2GameClients, this, &CPlugin::Hook_ClientPutInServer, true);

	g_CAddresses->Clean();

	delete g_CAddresses;
	delete g_CGameConfig;
	delete g_CFileManager;

	FlushAllDetours();

	return true;
}

void CPlugin::Hook_ClientPutInServer(CPlayerSlot slot, char const *pszName, int type, uint64 xuid)
{
	const char *steamid = g_pEngine->GetPlayerNetworkIDString(slot.Get());
	const char *clantag = g_CFileManager->GetClantagFromAuthid(steamid);

	if (!clantag)
	{
		return;
	}

	CBasePlayerController *pPlayerController = (CBasePlayerController *)g_pEntitySystem->GetBaseEntity((CEntityIndex)(slot.Get() + 1));
	g_CAddresses->SetClanTag(pPlayerController, clantag);
}

void CPlugin::Hook_StartupServer(const GameSessionConfiguration_t &config, ISource2WorldSession *, const char *)
{
	g_pGlobals = GetServerGlobals();

	if (g_pGlobals == nullptr)
	{
		Error("Failed to lookup g_pGlobals\n");
	}

	g_pEntitySystem = g_gGameResourceServiceServe->GetGameEntitySystem();
}

void CPlugin::AllPluginsLoaded()
{
}

bool CPlugin::Pause(char *error, size_t maxlen)
{
	return true;
}

bool CPlugin::Unpause(char *error, size_t maxlen)
{
	return true;
}

const char *CPlugin::GetLicense()
{
	return "Public Domain";
}

const char *CPlugin::GetVersion()
{
	return "1.0";
}

const char *CPlugin::GetDate()
{
	return __DATE__;
}

const char *CPlugin::GetLogTag()
{
	return "STUB";
}

const char *CPlugin::GetAuthor()
{
	return "VeryGames, Kriax";
}

const char *CPlugin::GetDescription()
{
	return PLUGIN_DESCRIPTION;
}

const char *CPlugin::GetName()
{
	return PLUGIN_NAME;
}

const char *CPlugin::GetURL()
{
	return "https://www.verygames.net";
}
