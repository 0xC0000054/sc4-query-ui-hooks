///////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-query-ui-hooks, a DLL Plugin for SimCity 4 that
// extends the query UI.
//
// Copyright (c) 2024 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
///////////////////////////////////////////////////////////////////////////////

#include "version.h"
#include "BuildingQueryHooks.h"
#include "BuildingQueryHookServer.h"
#include "BuildingQueryVariablesDllDirector.h"
#include "FloraQueryHooks.h"
#include "FloraQueryToolTipHookServer.h"
#include "NetworkQueryHooks.h"
#include "NetworkQueryToolTipHookServer.h"
#include "QueryToolTipDllDirector.h"
#include "FileSystem.h"
#include "GlobalHookServerPointers.h"
#include "Logger.h"
#include "SC4VersionDetection.h"
#include "Settings.h"
#include "cIGZApp.h"
#include "cIGZCmdLine.h"
#include "cIGZCOM.h"
#include "cIGZFrameWork.h"
#include "cIGZVariant.h"
#include "cIGZWin.h"
#include "cIGZWinMgr.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseString.h"
#include "cRZCOMDllDirector.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cISC4App.h"
#include "cISC4Occupant.h"
#include "GZServPtrs.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>

#include <Windows.h>
#include "wil/result.h"

static constexpr uint32_t kQueryDialogHooksDirectorID = 0x5EBF9B1E;

BuildingQueryHookServer* spBuildingQueryHookServer = nullptr;
FloraQueryToolTipHookServer* spFloraQueryToolTipHookServer = nullptr;
NetworkQueryToolTipHookServer* spNetworkQueryToolTipHookServer = nullptr;

namespace
{
	void InstallQueryUIHooks(const ISettings& settings)
	{
		Logger& logger = Logger::GetInstance();

		const uint16_t gameVersion = SC4VersionDetection::GetInstance().GetGameVersion();

		if (gameVersion == 641)
		{
			try
			{
				BuildingQueryHooks::Install(settings);
				NetworkQueryHooks::Install();
				FloraQueryHooks::Install();

				logger.WriteLine(LogLevel::Info, "Installed the query UI hooks.");
			}
			catch (const std::exception& e)
			{
				logger.WriteLineFormatted(
					LogLevel::Error,
					"Failed to install the query UI hooks: %s",
					e.what());
			}
		}
		else
		{
			logger.WriteLineFormatted(
				LogLevel::Error,
				"Failed to install the query UI hooks. Requires "
				"game version 641, found game version %d",
				gameVersion);
		}
	}
}


class QueryUIHooksDllDirector final : public cRZCOMDllDirector
{
public:
	QueryUIHooksDllDirector()
		: settings(),
		  buildingQueryVariablesDirector(settings)
	{
		// A child directors that are used to test the services provided
		// by this director.
		AddDirector(&buildingQueryVariablesDirector);
		AddDirector(&queryToolTipDirector);

		spBuildingQueryHookServer = &buildingQueryHookServer;
		spFloraQueryToolTipHookServer = &floraQueryToolTipHookServer;
		spNetworkQueryToolTipHookServer = &networkQueryToolTipHookServer;

		Logger& logger = Logger::GetInstance();
		logger.Init(FileSystem::GetLogFilePath(), LogLevel::Info, false);
		logger.WriteLogFileHeader("SC4QueryUIHooks v" PLUGIN_VERSION_STR);
	}

	bool GetClassObject(uint32_t rclsid, uint32_t riid, void** ppvObj)
	{
		bool result = false;

		if (rclsid == GZCLSID_cIBuildingQueryHookServer)
		{
			result = buildingQueryHookServer.QueryInterface(riid, ppvObj);
		}
		else if (rclsid == GZCLSID_cINetworkQueryToolTipHookServer)
		{
			result = networkQueryToolTipHookServer.QueryInterface(riid, ppvObj);
		}
		else if (rclsid == GZCLSID_cIFloraQueryToolTipHookServer)
		{
			result = floraQueryToolTipHookServer.QueryInterface(riid, ppvObj);
		}

		return result;
	}

	void EnumClassObjects(ClassObjectEnumerationCallback pCallback, void* pContext)
	{
		// We only support game version 641, so don't bother registering the GZCOM
		// classes on older game versions.
		// This is also a more obvious error behavior for DLL plugin developers than
		// having the classes be present, but not working.

		const uint16_t gameVersion = SC4VersionDetection::GetInstance().GetGameVersion();

		if (gameVersion == 641)
		{
			pCallback(GZCLSID_cIBuildingQueryHookServer, 0, pContext);
			pCallback(GZCLSID_cINetworkQueryToolTipHookServer, 0, pContext);
			pCallback(GZCLSID_cIFloraQueryToolTipHookServer, 0, pContext);
		}
	}

	uint32_t GetDirectorID() const
	{
		return kQueryDialogHooksDirectorID;
	}

	bool OnStart(cIGZCOM* pCOM)
	{
		settings.Load();

		InstallQueryUIHooks(settings);

		buildingQueryVariablesDirector.OnStart(pCOM);
		queryToolTipDirector.OnStart(pCOM);

		return true;
	}

private:
	BuildingQueryHookServer buildingQueryHookServer;
	BuildingQueryVariablesDllDirector buildingQueryVariablesDirector;
	FloraQueryToolTipHookServer floraQueryToolTipHookServer;
	NetworkQueryToolTipHookServer networkQueryToolTipHookServer;
	QueryToolTipDllDirector queryToolTipDirector;
	Settings settings;
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static QueryUIHooksDllDirector sDirector;
	return &sDirector;
}