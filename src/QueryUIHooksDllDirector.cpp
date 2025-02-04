///////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-query-ui-hooks, a DLL Plugin for SimCity 4 that
// extends the query UI.
//
// Copyright (c) 2024, 2025 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
///////////////////////////////////////////////////////////////////////////////

#include "version.h"
#include "BuildingQueryHooks.h"
#include "BuildingQueryHookServer.h"
#include "BuildingQueryVariablesProvider.h"
#include "FloraQueryHooks.h"
#include "FloraQueryToolTipHookServer.h"
#include "NetworkQueryHooks.h"
#include "NetworkQueryToolTipHookServer.h"
#include "PropQueryHooks.h"
#include "PropQueryToolTipHookServer.h"
#include "QueryToolTipProvider.h"
#include "TerrainQueryHooks.h"
#include "FileSystem.h"
#include "GlobalHookServerPointers.h"
#include "GlobalSC4InterfacePointers.h"
#include "Logger.h"
#include "SC4VersionDetection.h"
#include "Settings.h"
#include "cIGZApp.h"
#include "cIGZCmdLine.h"
#include "cIGZCOM.h"
#include "cIGZFrameWork.h"
#include "cIGZMessage2Standard.h"
#include "cIGZMessageServer2.h"
#include "cIGZVariant.h"
#include "cIGZWin.h"
#include "cIGZWinMgr.h"
#include "cISC4App.h"
#include "cISC4City.h"
#include "cISC4Occupant.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseString.h"
#include "cRZMessage2COMDirector.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "GZServPtrs.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>

#include <Windows.h>
#include "wil/result.h"

static constexpr uint32_t kSC4MessagePostCityInit = 0x26D31EC1;
static constexpr uint32_t kSC4MessagePreCityShutdown = 0x26D31EC2;

static constexpr std::array<uint32_t, 2> RequiredNotifications =
{
	kSC4MessagePostCityInit,
	kSC4MessagePreCityShutdown
};

static constexpr uint32_t kQueryDialogHooksDirectorID = 0x5EBF9B1E;

BuildingQueryHookServer* spBuildingQueryHookServer = nullptr;
FloraQueryToolTipHookServer* spFloraQueryToolTipHookServer = nullptr;
NetworkQueryToolTipHookServer* spNetworkQueryToolTipHookServer = nullptr;
PropQueryToolTipHookServer* spPropQueryToolTipHookServer = nullptr;

cISC4FlammabilitySimulator* spFlammabilitySimulator = nullptr;
cISC4LandValueSimulator* spLandValueSimulator = nullptr;
cISC4PollutionSimulator* spPollutionSimulator = nullptr;
cISC4WeatherSimulator* spWeatherSimulator = nullptr;

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
				PropQueryHooks::Install();
				TerrainQueryHooks::Install();

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


class QueryUIHooksDllDirector final : public cRZMessage2COMDirector
{
public:
	QueryUIHooksDllDirector()
		: settings(),
		  buildingQueryVariablesProvider(settings)
	{
		spBuildingQueryHookServer = &buildingQueryHookServer;
		spFloraQueryToolTipHookServer = &floraQueryToolTipHookServer;
		spNetworkQueryToolTipHookServer = &networkQueryToolTipHookServer;
		spPropQueryToolTipHookServer = &propQueryToolTipHookServer;

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
		else if (rclsid == GZCLSID_cIPropQueryToolTipHookServer)
		{
			result = propQueryToolTipHookServer.QueryInterface(riid, ppvObj);
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
			pCallback(GZCLSID_cIPropQueryToolTipHookServer, 0, pContext);
		}
	}

	uint32_t GetDirectorID() const
	{
		return kQueryDialogHooksDirectorID;
	}

	void PostCityInit(cIGZMessage2Standard* pStandardMsg)
	{
		cISC4City* pCity = static_cast<cISC4City*>(pStandardMsg->GetVoid1());

		if (pCity)
		{
			spFlammabilitySimulator = pCity->GetFlammabilitySimulator();
			spLandValueSimulator = pCity->GetLandValueSimulator();
			spPollutionSimulator = pCity->GetPollutionSimulator();
			spWeatherSimulator = pCity->GetWeatherSimulator();
		}

		buildingQueryVariablesProvider.PostCityInit(pStandardMsg, mpCOM);
		queryToolTipProvider.PostCityInit(pStandardMsg, mpCOM);
	}

	void PreCityShutdown(cIGZMessage2Standard* pStandardMsg)
	{
		spFlammabilitySimulator = nullptr;
		spLandValueSimulator = nullptr;
		spPollutionSimulator = nullptr;
		spWeatherSimulator = nullptr;
		buildingQueryVariablesProvider.PreCityShutdown(pStandardMsg, mpCOM);
		queryToolTipProvider.PreCityShutdown(pStandardMsg, mpCOM);
	}

	bool DoMessage(cIGZMessage2* pMsg)
	{
		cIGZMessage2Standard* pStandardMsg = static_cast<cIGZMessage2Standard*>(pMsg);

		switch (pMsg->GetType())
		{
		case kSC4MessagePostCityInit:
			PostCityInit(pStandardMsg);
			break;
		case kSC4MessagePreCityShutdown:
			PreCityShutdown(pStandardMsg);
			break;
		}

		return true;
	}

	bool OnStart(cIGZCOM* pCOM)
	{
		mpFrameWork->AddHook(this);

		settings.Load();

		InstallQueryUIHooks(settings);

		return true;
	}

	bool PostAppInit()
	{
		cIGZMessageServer2Ptr pMsgServ;

		if (pMsgServ)
		{
			for (uint32_t messageID : RequiredNotifications)
			{
				pMsgServ->AddNotification(this, messageID);
			}
		}

		buildingQueryVariablesProvider.PostAppInit(mpCOM);
		queryToolTipProvider.PostAppInit(mpCOM);

		return true;
	}

	bool PreAppShutdown()
	{
		buildingQueryVariablesProvider.PreAppShutdown(mpCOM);
		queryToolTipProvider.PreAppShutdown(mpCOM);

		return true;
	}

private:
	BuildingQueryHookServer buildingQueryHookServer;
	BuildingQueryVariablesProvider buildingQueryVariablesProvider;
	FloraQueryToolTipHookServer floraQueryToolTipHookServer;
	NetworkQueryToolTipHookServer networkQueryToolTipHookServer;
	PropQueryToolTipHookServer propQueryToolTipHookServer;
	QueryToolTipProvider queryToolTipProvider;
	Settings settings;
};

cRZCOMDllDirector* RZGetCOMDllDirector() {
	static QueryUIHooksDllDirector sDirector;
	return &sDirector;
}