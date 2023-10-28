#include "KeyValues.h"
#include "abstract.h"
#include <string>
#include <unordered_map>

class CFileManager
{
public:
    CFileManager()
    {

    }

    bool Init(IFileSystem *filesystem);
    const char *GetClantagFromAuthid(const char *steamid);

private:
    KeyValues *g_kvPlayers;
    std::unordered_map<std::string, std::string> m_clantags;
};