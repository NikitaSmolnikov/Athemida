#pragma once
#include <Windows.h>
#include <Psapi.h>
#pragma comment (lib, "Psapi.lib")
class SigScan
{
public:
	static MODULEINFO GetModuleInfo(const char* szModule)
	{
		MODULEINFO modinfo = { 0 };
		HMODULE hModule = GetModuleHandleA(szModule);
		if (hModule == 0) return modinfo;
		K32GetModuleInformation(GetCurrentProcess(), hModule, &modinfo, sizeof(MODULEINFO));
		return modinfo;
	}
	static DWORD FindPattern(const char* module, const char* pattern, const char* mask)
	{
		auto lowercase = [](std::string data) -> std::string {
			std::transform(data.begin(), data.end(), data.begin(), [](unsigned char c) { return std::tolower(c); });
			return data;
		};
		if (lowercase(module) == "kernel32.dll") return NULL;
		MODULEINFO mInfo = GetModuleInfo(module);
		DWORD base = (DWORD)mInfo.lpBaseOfDll;
		DWORD size = (DWORD)mInfo.SizeOfImage;
		DWORD patternLength = (DWORD)strlen(mask);
		for (DWORD i = 0; i < size - patternLength; i++)
		{
			bool found = true;
			for (DWORD j = 0; j < patternLength; j++)
			{
				//if ((DWORD)(base + i + j) > (DWORD)mInfo.SizeOfImage) { found = false; break; }
				found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
			}
			if (found)
			{
				return base + i;
			}
		}
		return NULL;
	}
	// experimental
	static DWORD FindPatternExplicit(DWORD base, DWORD size, const char* pattern, const char* mask)
	{
		DWORD patternLength = (DWORD)strlen(mask);
		for (DWORD i = 0; i < size - patternLength; i++)
		{
			bool found = true;
			for (DWORD j = 0; j < patternLength; j++) {
				found &= mask[j] == '?' || pattern[j] == *(char*)(base + i + j);
			}
			if (found)
			{
				return (base + i);
			}
		}
	}
};