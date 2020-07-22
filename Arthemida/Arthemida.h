﻿#pragma once
#include <Windows.h>
#include <stdio.h>
#include <thread>
#include <vector>
#include <string>
#include <map>
#include <tuple>
#include <winternl.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <conio.h>
#include <algorithm>
#include "Utils.h"
#include "GameHooks.h"
#ifdef _WIN64
#define START_ADDRESS (PVOID)0x00000000010000
#define END_ADDRESS (0x00007FF8F2580000 - 0x00000000010000)
#else
#define START_ADDRESS (PVOID)0x10000
#define END_ADDRESS (0x7FFF0000 - 0x10000)
#endif
namespace ART_LIB
{
	class ArtemisLibrary : public GameHooks
	{
	public:
		enum class DetectionType
		{
			ART_ILLEGAL_THREAD = 1,
			ART_ILLEGAL_MODULE = 2,
			ART_FAKE_LAUNCHER = 3,
			ART_APC_INJECTION = 4,
			ART_RETURN_ADDRESS = 5,
			ART_MANUAL_MAP = 6,
			ART_INLINE_HOOK = 7,
			ART_MEMORY_CHANGED = 8
		};
		typedef DWORD(__stdcall* LPFN_GetMappedFileNameA)(HANDLE hProcess, LPVOID lpv, LPCSTR lpFilename, DWORD nSize);
		struct ARTEMIS_DATA
		{
			PVOID baseAddr;
			SIZE_T regionSize;
			DWORD MemoryRights;
			DetectionType type;
			std::string dllName;
			std::string dllPath;
			std::tuple<PVOID, PCONTEXT, const char*> ApcInfo;
		};
		typedef void(__stdcall* ArtemisCallback)(ARTEMIS_DATA* artemis);
		struct ArtemisConfig
		{
			HANDLE hSelfModule = nullptr;
			std::multimap<DWORD, std::string> ModuleSnapshot;
			LPFN_GetMappedFileNameA lpGetMappedFileNameA = nullptr;
			ArtemisCallback callback = nullptr;
			std::vector<PVOID> ExcludedThreads;
			bool DetectThreads = false;
			volatile bool ThreadScanner = false;
			volatile bool ModuleScanner = false;
			DWORD ThreadScanDelay = 0x0;
			std::vector<PVOID> ExcludedModules;
			std::vector<PVOID> ExcludedImages;
			bool DetectModules = false;
			DWORD ModuleScanDelay = 0x0;
			DWORD MemoryScanDelay = 0x0;
			bool DetectFakeLaunch = false;
			bool DetectAPC = false;
			bool DetectReturnAddresses = false;
			bool DetectManualMap = false;
			bool DetectInlineHooks = false;
			bool DetectMemoryPatch = false;
			std::vector<std::string> ModulesWhitelist;
		};
		static void DumpExportTable(HMODULE hModule, std::multimap<PVOID, std::string>& ExportsList);
		static void __stdcall ScanForDllThreads(ArtemisConfig* cfg);
		static void __stdcall ModuleScanner(ArtemisConfig* cfg);
		static bool __stdcall InstallApcDispatcher(ArtemisConfig* cfg);
		static void __stdcall MemoryScanner(ArtemisConfig* cfg);
		static bool __stdcall InstallGameHooks(ArtemisConfig* cfg);
		static bool __stdcall DeleteGameHooks(void);
		static bool __stdcall DisableArtemis(void);
		static ArtemisLibrary* __cdecl ReloadArtemis(ArtemisConfig* cfg);
		static void __stdcall CheckLauncher(ArtemisConfig* cfg);
	};
};