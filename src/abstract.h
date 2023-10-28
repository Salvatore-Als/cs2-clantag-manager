#pragma once

#include <ISmmPlugin.h>
#include <igameevents.h>
#include <iplayerinfo.h>
#include <sh_vector.h>
#include <stdint.h>
#include <playerslot.h>
#include "networksystem/inetworkserializer.h"
#include <iserver.h>

#define PREFIX "ClanTag Manager"
#define PLUGIN_NAME "VeryGames ClanTag Manager"
#define PLUGIN_DESCRIPTION "Manage clantag by using custom config file"

#define GAMEDATA_PATH "addons/vgclantagmanager/gamedata/vgclantagmanager.games.txt"
#define CONFIG_CLANTAGS_PATH "addons/vgclantagmanager/configs/clantags.ini"
#define CONFIG_PLAYERS_PATH "addons/vgclantagmanager/configs/players.ini"

#ifdef _WIN32
#define ROOTBIN "/bin/win64/"
#define GAMEBIN "/csgo/bin/win64/"
#define MODULE_PREFIX ""
#define MODULE_EXT ".dll"
#else
#define ROOTBIN "/bin/linuxsteamrt64/"
#define GAMEBIN "/csgo/bin/linuxsteamrt64/"
#define MODULE_PREFIX "lib"
#define MODULE_EXT ".so"
#endif

void Debug(const char *, ...);
void Fatal(const char *, ...);
void Warn(const char *, ...);