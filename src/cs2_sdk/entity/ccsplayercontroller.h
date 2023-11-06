#pragma once

#include "cbaseplayercontroller.h"

class CBasePlayerController;

class CCSPlayerController : public CBasePlayerController
{
public:
    DECLARE_SCHEMA_CLASS(CCSPlayerController);

    SCHEMA_FIELD(CUtlSymbolLarge, m_szClan);
};