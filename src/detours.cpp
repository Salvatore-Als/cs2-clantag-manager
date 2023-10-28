#include "cdetour.h"
#include "module.h"
#include "addresses.h"
#include "detours.h"
#include "gameconfig.h"
#include "abstract.h"
#include "entity/cbaseplayercontroller.h"

extern CEntitySystem *g_pEntitySystem;
extern IGameEventManager2 *g_pGameEventManager;

/*
DECLARE_DETOUR(ClientSettingsChanged, Detour_ClientSettingsChanged);

void FASTCALL Detour_ClientSettingsChanged(CGameRules *pGameRules, CBasePlayerController *pPlayerController)
{
	// TODO: It's to save time, I suppose that when valve adds the clantags into the game I will need to manage here
}
*/

CUtlVector<CDetourBase *> g_vecDetours;

bool InitDetours(CGameConfig *gameConfig)
{
	bool success = true;

	g_vecDetours.PurgeAndDeleteElements();

	/*
	if (!ClientSettingsChanged.CreateDetour(gameConfig))
	{
		success = false;
	}

	ClientSettingsChanged.EnableDetour();
	*/

	return success;
}

void FlushAllDetours()
{
	g_vecDetours.Purge();
}