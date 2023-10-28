// from cs2fixes

#pragma once
#include "../abstract.h"
#include "dbg.h"
#include "interface.h"
#include "strtools.h"
#include <cstdint>
#include "metamod_oslink.h"
#include "plat.h"

#ifdef _WIN32
#include <Psapi.h>
#endif

class CModule
{
public:
	CModule(const char *path, const char *module) : m_pszModule(module), m_pszPath(path)
	{
		char szModule[MAX_PATH];

		V_snprintf(szModule, MAX_PATH, "%s%s%s%s%s", Plat_GetGameDirectory(), path, MODULE_PREFIX, m_pszModule, MODULE_EXT);

		m_hModule = dlmount(szModule);

		if (!m_hModule)
			Fatal("Could not find %s\n", szModule);

#ifdef _WIN32
		MODULEINFO m_hModuleInfo;
		GetModuleInformation(GetCurrentProcess(), m_hModule, &m_hModuleInfo, sizeof(m_hModuleInfo));

		m_base = (void *)m_hModuleInfo.lpBaseOfDll;
		m_size = m_hModuleInfo.SizeOfImage;
#else
		if (int e = GetModuleInformation(m_hModule, &m_base, &m_size))
			Fatal("Failed to get module info for %s, error %d\n", szModule, e);
#endif
	}

	void *FindSignature(const byte *pData, size_t iSigLength)
	{
		unsigned char *pMemory;
		void *return_addr = nullptr;

		pMemory = (byte *)m_base;

		for (size_t i = 0; i < m_size; i++)
		{
			size_t Matches = 0;
			while (*(pMemory + i + Matches) == pData[Matches] || pData[Matches] == '\x2A')
			{
				Matches++;
				if (Matches == iSigLength)
					return_addr = (void *)(pMemory + i);
			}
		}

		return return_addr;
	}

	void *FindInterface(const char *name)
	{
		CreateInterfaceFn fn = (CreateInterfaceFn)dlsym(m_hModule, "CreateInterface");

		if (!fn)
			Fatal("Could not find CreateInterface in %s\n", m_pszModule);

		void *pInterface = fn(name, nullptr);

		if (!pInterface)
			Fatal("Could not find %s in %s\n", name, m_pszModule);

		return pInterface;
	}

	const char *m_pszModule;
	const char *m_pszPath;
	HINSTANCE m_hModule;
	void *m_base;
	size_t m_size;
};