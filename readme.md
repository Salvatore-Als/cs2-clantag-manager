[![VG Logo](https://www.verygames.net/img/logo_vg.png)](https://www.verygames.net/en)

## Overview

**VeryGames ClanTag Manager** is a MM plugin designed to help you manage player clantags within the scoreboard in your game server. 
This plugin allows you to easily assign and display clantags for players based on their configuration.

### 1. Installation

Download the release and drag&drop on `/game/csgo/` folder

### 2. Configuration `/addons/vgclantagmanager/configs/clantags.ini`

The `clantags.ini` file contains the list of available clantags and their corresponding identifiers. You can define various clantags as follows:

```
"Config"
{
    "0"
    {
        "id"        "clantagid"
        "clantag"   "[CLANTAG]"
    }
    "1"
    {
        "id"        "vip"
        "clantag"   "[VIP]"
    }
    "2"
    {
        "id"        "admin"
        "clantag"   "[ADMIN]"
    }
}
```

### 3. Configuration `/addons/vgclantagmanager/configs/players.ini`

The players.ini file contains a mapping of players' identifiers (such as STEAMIDs) to the corresponding clantag they should be assigned. Here's an example:

```
"Players"
{
    "STEAMID 3"      "clantagid"
    
    "[U:1:58669672]" "admin"
    "[U:1:58669673]" "root"
    "[U:1:58669676]" "vip"
}
```

---

Thanks to CS2Fixes project (Poggu & Xen) for providing a code for managing adresses and detours
