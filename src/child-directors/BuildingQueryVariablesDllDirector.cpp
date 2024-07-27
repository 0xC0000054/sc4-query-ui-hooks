///////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-query-ui-hooks, a DLL Plugin for SimCity 4 that
// provides more data for the query UI.
//
// Copyright (c) 2024 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
///////////////////////////////////////////////////////////////////////////////

#include "BuildingQueryVariablesDllDirector.h"
#include "cIBuildingStyleInfo.h"
#include "cIBuildingQueryHookServer.h"
#include "DebugUtil.h"
#include "Logger.h"
#include "cIGZApp.h"
#include "cIGZCOM.h"
#include "cIGZFrameWork.h"
#include "cIGZLanguageManager.h"
#include "cIGZLanguageUtility.h"
#include "cIGZMessage2Standard.h"
#include "cIGZMessageServer2.h"
#include "cIGZString.h"
#include "cIGZVariant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cISCStringDetokenizer.h"
#include "cISC4App.h"
#include "cISC4BuildingDevelopmentSimulator.h"
#include "cISC4City.h"
#include "cISC4Lot.h"
#include "cISC4LotConfiguration.h"
#include "cISC4LotManager.h"
#include "cISC4Occupant.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseString.h"
#include "cS3DVector3.h"
#include <GZServPtrs.h>

#include <algorithm>
#include <array>
#include <any>
#include <cctype>
#include <filesystem>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>

static constexpr uint32_t kSC4MessagePostCityInit = 0x26D31EC1;
static constexpr uint32_t kSC4MessagePreCityShutdown = 0x26D31EC2;

static constexpr uint32_t kBuildingQueryVariablesDirectorID = 0x1317A7E6;

using namespace std::string_view_literals;

namespace
{
	struct UnknownTokenContext
	{
		cIGZCOM* pCOM;
		cISC4Occupant* pOccupant;
		cISC4LotManager* lotManager;

		UnknownTokenContext(
			cIGZCOM* GZCOM,
			cISC4Occupant* occupant,
			cISC4LotManager* lotManager)
			: pCOM(GZCOM),
			  pOccupant(occupant),
			  lotManager(lotManager)
		{
		}
	};

	static constexpr std::array<std::pair<uint32_t, std::string_view>, 4> MaxisBuildingStyles =
	{
		std::pair(0x2000, "Chicago 1890"),
		std::pair(0x2001, "New York 1940"),
		std::pair(0x2002, "Houston 1990"),
		std::pair(0x2003, "Euro-Contemporary"),
	};

	bool MakeNumberStringForCurrentLanguage(int64_t number, cIGZString& destination)
	{
		bool result = false;

		cIGZLanguageManagerPtr pLM;

		if (pLM)
		{
			cIGZLanguageUtility* pLU = pLM->GetLanguageUtility(pLM->GetCurrentLanguage());

			if (pLU)
			{
				result = pLU->MakeNumberString(number, destination);
			}
		}

		return result;
	}

	bool CheckForDefaultMaxisBuildingStyles(cISC4Occupant* pOccupant, cIGZString& destination)
	{
		bool result = false;

		if (pOccupant)
		{
			constexpr size_t lastStyleIndex = MaxisBuildingStyles.size() - 1;
			const std::string_view seperator(", ");

			for (size_t i = 0; i < MaxisBuildingStyles.size(); i++)
			{
				const auto& item = MaxisBuildingStyles[i];

				if (pOccupant->IsOccupantGroup(item.first))
				{
					destination.Append(item.second.data(), item.second.size());

					if (i < lastStyleIndex)
					{
						destination.Append(seperator.data(), seperator.size());
					}
					result = true;
				}
			}
		}

		return result;
	}

	bool GetBuildingStylesToken(const UnknownTokenContext* context, cIGZString& outReplacement)
	{
		bool result = false;

		if (context && context->pCOM)
		{
			cRZAutoRefCount<cIBuildingStyleInfo> pBuildingStyleInfo;

			if (context->pCOM->GetClassObject(
				GZCLSID_cIBuildingStyleInfo,
				GZIID_cIBuildingStyleInfo,
				pBuildingStyleInfo.AsPPVoid()))
			{
				// If the More Building Styles DLL is installed we check the building's OccupantGroups
				// for any styles that are present in the Building Style Control.
				// The names of the styles are placed in the output string as comma-separated values.
				result = pBuildingStyleInfo->GetBuildingStyleNames(context->pOccupant, outReplacement);
			}
			else
			{
				// If the More Building Styles DLL is not installed we fall back to checking for
				// the 4 built-in Maxis styles.
				result = CheckForDefaultMaxisBuildingStyles(context->pOccupant, outReplacement);
			}
		}

		return result;
	}

	bool GetCapacityToken(const UnknownTokenContext* context, cIGZString& outReplacement, uint32_t developerType)
	{
		bool result = false;

		if (context && context->pOccupant && context->lotManager)
		{
			cISC4Lot* pLot = context->lotManager->GetOccupantLot(context->pOccupant);

			if (pLot)
			{
				uint16_t capacity = pLot->GetCapacity(developerType, true);
				result = MakeNumberStringForCurrentLanguage(capacity, outReplacement);
			}
		}

		return result;
	}

	bool GetGrowthStageToken(const UnknownTokenContext* context, cIGZString& outReplacement)
	{
		bool result = false;

		if (context && context->pOccupant && context->lotManager)
		{
			cISC4Lot* pLot = context->lotManager->GetOccupantLot(context->pOccupant);

			if (pLot)
			{
				cISC4LotConfiguration* pLotConfiguration = pLot->GetLotConfiguration();

				if (pLotConfiguration)
				{
					uint8_t growthStage = pLotConfiguration->GetGrowthStage();
					result = MakeNumberStringForCurrentLanguage(growthStage, outReplacement);
				}
			}
		}

		return result;
	}

	bool GetOccupancyToken(const UnknownTokenContext* context, cIGZString& outReplacement, uint32_t developerType)
	{
		bool result = false;

		if (context && context->pOccupant && context->lotManager)
		{
			cISC4Lot* pLot = context->lotManager->GetOccupantLot(context->pOccupant);

			if (pLot)
			{
				uint16_t occupancy = pLot->GetPopulation(developerType);
				result = MakeNumberStringForCurrentLanguage(occupancy, outReplacement);
			}
		}

		return result;
	}
}

using TokenDataCallback = std::function<bool(const UnknownTokenContext*, cIGZString&)>;

using namespace std::placeholders;

static const std::unordered_map<std::string_view, TokenDataCallback> tokenDataCallbacks =
{
	{ "building_styles", GetBuildingStylesToken },
	{ "growth_stage", GetGrowthStageToken },
	{ "r1_occupancy", std::bind(GetOccupancyToken, _1, _2, 0x1010) },
	{ "r1_capacity", std::bind(GetCapacityToken, _1, _2, 0x1010) },
	{ "r2_occupancy", std::bind(GetOccupancyToken, _1, _2, 0x1020) },
	{ "r2_capacity", std::bind(GetCapacityToken, _1, _2, 0x1020) },
	{ "r3_occupancy", std::bind(GetOccupancyToken, _1, _2, 0x1030) },
	{ "r3_capacity", std::bind(GetCapacityToken, _1, _2, 0x1030) },
	{ "cs1_occupancy", std::bind(GetOccupancyToken, _1, _2, 0x3110) },
	{ "cs1_capacity", std::bind(GetCapacityToken, _1, _2, 0x3110) },
	{ "cs2_occupancy", std::bind(GetOccupancyToken, _1, _2, 0x3120) },
	{ "cs2_capacity", std::bind(GetCapacityToken, _1, _2, 0x3120) },
	{ "cs3_occupancy", std::bind(GetOccupancyToken, _1, _2, 0x3130) },
	{ "cs3_capacity", std::bind(GetCapacityToken, _1, _2, 0x3130) },
	{ "co2_occupancy", std::bind(GetOccupancyToken, _1, _2, 0x3320) },
	{ "co2_capacity", std::bind(GetCapacityToken, _1, _2, 0x3320) },
	{ "co3_occupancy", std::bind(GetOccupancyToken, _1, _2, 0x3330) },
	{ "co3_capacity", std::bind(GetCapacityToken, _1, _2, 0x3330) },
	{ "ir_occupancy", std::bind(GetOccupancyToken, _1, _2, 0x4100) },
	{ "ir_capacity", std::bind(GetCapacityToken, _1, _2, 0x4100) },
	{ "id_occupancy", std::bind(GetOccupancyToken, _1, _2, 0x4200) },
	{ "id_capacity", std::bind(GetCapacityToken, _1, _2, 0x4200) },
	{ "im_occupancy", std::bind(GetOccupancyToken, _1, _2, 0x4300) },
	{ "im_capacity", std::bind(GetCapacityToken, _1, _2, 0x4300) },
	{ "iht_occupancy", std::bind(GetOccupancyToken, _1, _2, 0x4400) },
	{ "iht_capacity", std::bind(GetCapacityToken, _1, _2, 0x4400) },
};

static bool UnknownTokenCallback(cIGZString const& token, cIGZString& outReplacement, void* pContext)
{
	bool result = false;

	const std::string_view tokenAsStringView(token.Data(), token.Strlen());

	const auto& entry = tokenDataCallbacks.find(tokenAsStringView);

	if (entry != tokenDataCallbacks.end())
	{
		const UnknownTokenContext* context = static_cast<UnknownTokenContext*>(pContext);

		// The string may have been set to an error message by some other token callback method.
		outReplacement.Erase(0, outReplacement.Strlen());

		result = entry->second(context, outReplacement);
	}

	return result;
}

BuildingQueryVariablesDllDirector::BuildingQueryVariablesDllDirector()
	: stringDetokenizer(nullptr),
	  lotManager(nullptr)
{
}

bool BuildingQueryVariablesDllDirector::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIBuildingQueryHookTarget)
	{
		*ppvObj = static_cast<cIBuildingQueryDialogHookTarget*>(this);
		AddRef();

		return true;
	}

	return cRZMessage2COMDirector::QueryInterface(riid, ppvObj);
}

uint32_t BuildingQueryVariablesDllDirector::AddRef()
{
	return cRZMessage2COMDirector::AddRef();
}

uint32_t BuildingQueryVariablesDllDirector::Release()
{
	return cRZMessage2COMDirector::Release();
}

uint32_t BuildingQueryVariablesDllDirector::GetDirectorID() const
{
	return kBuildingQueryVariablesDirectorID;
}

bool BuildingQueryVariablesDllDirector::OnStart(cIGZCOM* pCOM)
{
	cIGZFrameWork* const pFramework = pCOM->FrameWork();

	if (pFramework->GetState() < cIGZFrameWork::kStatePreAppInit)
	{
		pFramework->AddHook(this);
	}
	else
	{
		PreAppInit();
	}

	return true;
}

bool BuildingQueryVariablesDllDirector::PostAppInit()
{
	cISC4AppPtr pSC4App;

	if (pSC4App)
	{
		stringDetokenizer = pSC4App->GetStringDetokenizer();
	}

	cIGZMessageServer2Ptr pMsgServ;

	if (pMsgServ)
	{
		std::vector<uint32_t> requiredNotifications;
		requiredNotifications.push_back(kSC4MessagePostCityInit);
		requiredNotifications.push_back(kSC4MessagePreCityShutdown);

		for (uint32_t messageID : requiredNotifications)
		{
			pMsgServ->AddNotification(this, messageID);
		}
	}

	return true;
}

bool BuildingQueryVariablesDllDirector::DoMessage(cIGZMessage2* pMsg)
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

void BuildingQueryVariablesDllDirector::PostCityInit(cIGZMessage2Standard* pStandardMsg)
{
	cISC4City* pCity = static_cast<cISC4City*>(pStandardMsg->GetVoid1());

	lotManager = pCity->GetLotManager();

	cIGZCOM* const pCOM = GZCOM();

	cRZAutoRefCount<cIBuildingQueryHookServer> hookServer;

	if (pCOM->GetClassObject(
		GZCLSID_cIBuildingQueryHookServer,
		GZIID_cIBuildingQueryHookServer,
		hookServer.AsPPVoid()))
	{
		hookServer->AddNotification(this);
	}
}

void BuildingQueryVariablesDllDirector::PreCityShutdown(cIGZMessage2Standard* pStandardMsg)
{
	lotManager = nullptr;

	cIGZCOM* const pCOM = GZCOM();

	cRZAutoRefCount<cIBuildingQueryHookServer> hookServer;

	if (pCOM->GetClassObject(
		GZCLSID_cIBuildingQueryHookServer,
		GZIID_cIBuildingQueryHookServer,
		hookServer.AsPPVoid()))
	{
		hookServer->RemoveNotification(this);
	}
}

void BuildingQueryVariablesDllDirector::BeforeDialogShown(cISC4Occupant* pOccupant)
{
	if (stringDetokenizer)
	{
		UnknownTokenContext context(mpCOM, pOccupant, lotManager);

		stringDetokenizer->AddUnknownTokenReplacementMethod(&UnknownTokenCallback, &context, true);

#ifdef _DEBUG
		DebugLogTokenizerVariables();
#endif // _DEBUG
	}
}

void BuildingQueryVariablesDllDirector::AfterDialogShown(cISC4Occupant* pOccupant)
{
	if (stringDetokenizer)
	{
		UnknownTokenContext context(mpCOM, pOccupant, lotManager);

		stringDetokenizer->AddUnknownTokenReplacementMethod(&UnknownTokenCallback, &context, false);
	}
}

void BuildingQueryVariablesDllDirector::DebugLogTokenizerVariables()
{
	for (const auto& entry : tokenDataCallbacks)
	{
		cRZBaseString token("#", 1);
		token.Append(entry.first.data(), entry.first.size());
		token.Append("#", 1);

		cRZBaseString result;

		if (stringDetokenizer->Detokenize(token, result))
		{
			DebugUtil::PrintLineToDebugOutputFormatted("%s = %s", token.ToChar(), result.ToChar());
		}
	}
}

