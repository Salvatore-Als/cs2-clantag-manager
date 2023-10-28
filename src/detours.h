#pragma once
#include "cdetour.h"

class CBasePlayerController;
class CGameRules;

bool InitDetours(CGameConfig *gameConfig);
void FlushAllDetours();

/*
void FASTCALL Detour_ClientSettingsChanged(CGameRules *pGameRules, CBasePlayerController *pPlayerController);
extern CDetour<decltype(Detour_ClientSettingsChanged)> ClientSettingsChanged;
*/