#include "filemanager.h"
#include "abstract.h"

bool CFileManager::Init(IFileSystem *filesystem)
{
    KeyValues *kvClantags = new KeyValues("Config");
    kvClantags->LoadFromFile(filesystem, CONFIG_CLANTAGS_PATH);
    KeyValues::AutoDelete autoDelete(kvClantags);

    if (!kvClantags)
    {
        Fatal("Unable to load clantags file %s", CONFIG_CLANTAGS_PATH);
        return false;
    }

    g_kvPlayers = new KeyValues("Players");
    g_kvPlayers->LoadFromFile(filesystem, CONFIG_PLAYERS_PATH);

    if (!g_kvPlayers)
    {
        Fatal("Unable to load players file %s", CONFIG_PLAYERS_PATH);
        return false;
    }

    for (KeyValues *pKey = kvClantags->GetFirstSubKey(); pKey; pKey = pKey->GetNextKey())
    {
        const char *id = pKey->GetString("id");
        const char *clantag = pKey->GetString("clantag");

        m_clantags[id] = clantag;
    }

    return true;
}

const char *CFileManager::GetClantagFromAuthid(const char *steamid)
{   
    const char *clantagId = g_kvPlayers->GetString(steamid, nullptr);

    if (!clantagId)
    {
        return nullptr;
    }

    auto clanTag = m_clantags.find(clantagId);

    if (clanTag == m_clantags.end())
    {
        return nullptr;
    }

    return clanTag->second.c_str();
}