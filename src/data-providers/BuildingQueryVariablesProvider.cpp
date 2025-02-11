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

#include "BuildingQueryVariablesProvider.h"
#include "cIBuildingStyleInfo2.h"
#include "cIBuildingQueryHookServer.h"
#include "DebugUtil.h"
#include "frozen/string.h"
#include "frozen/unordered_map.h"
#include "GZStringUtil.h"
#include "Logger.h"
#include "OccupantUtil.h"
#include "cGZPersistResourceKey.h"
#include "cIGZLanguageManager.h"
#include "cIGZLanguageUtility.h"
#include "cIGZMessage2Standard.h"
#include "cIGZPersistDBSegment.h"
#include "cIGZPersistDBSegmentMultiPackedFiles.h"
#include "cIGZPersistResourceManager.h"
#include "cIGZString.h"
#include "cIGZVariant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cISCStringDetokenizer.h"
#include "cISC4Advisor.h"
#include "cISC4App.h"
#include "cISC4BudgetSimulator.h"
#include "cISC4BuildingDevelopmentSimulator.h"
#include "cISC4City.h"
#include "cISC4Lot.h"
#include "cISC4LotConfiguration.h"
#include "cISC4LotManager.h"
#include "cISC4MySim.h"
#include "cISC4MySimAgentSimulator.h"
#include "cISC4Occupant.h"
#include "cISC4TrafficSimulator.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseString.h"
#include "cS3DVector3.h"
#include "GZServPtrs.h"
#include "SC4String.h"

#include <algorithm>
#include <array>
#include <any>
#include <cctype>
#include <filesystem>
#include <functional>
#include <memory>
#include <set>
#include <string>

using namespace std::string_view_literals;

namespace
{
	struct UnknownTokenContext
	{
		cISC4Occupant* pOccupant;
		cISC4City* pCity;

		UnknownTokenContext()
			: pOccupant(nullptr),
			  pCity(nullptr)
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

	cISC4Lot* GetOccupantLot(const UnknownTokenContext* context)
	{
		cISC4Lot* pLot = nullptr;

		if (context && context->pCity && context->pOccupant)
		{
			cISC4LotManager* pLotManager = context->pCity->GetLotManager();

			if (pLotManager)
			{
				pLot = pLotManager->GetOccupantLot(context->pOccupant);
			}
		}

		return pLot;
	}

	enum class NumberType
	{
		Number = 0,
		Money
	};

	bool MakeNumberStringForCurrentLanguage(
		int64_t number,
		cIGZString& destination,
		NumberType type = NumberType::Number)
	{
		bool result = false;

		cIGZLanguageManagerPtr pLM;

		if (pLM)
		{
			cIGZLanguageUtility* pLU = pLM->GetLanguageUtility(pLM->GetCurrentLanguage());

			if (pLU)
			{
				if (type == NumberType::Number)
				{
					result = pLU->MakeNumberString(number, destination);
				}
				else if (type == NumberType::Money)
				{
					// The currently symbol string is the hexadecimal-escaped UTF-8 encoding
					// of the section symbol (§).
					// The \xC2 value is the first byte in a two byte UTF-8 sequence, and the
					// \xA7 value is the Unicode value of the section symbol (U+00A7).
					// See the following page for more information on UTF-8 encoding:
					// https://www.fileformat.info/info/unicode/utf8.htm
					//
					// UTF-8 is SC4's native string encoding.

					static const cRZBaseString currencySymbol("\xC2\xA7");

					result = pLU->MakeMoneyString(number, destination, &currencySymbol);
				}
			}
		}

		return result;
	}

	bool CopySC4StringValue(const SC4String* pSC4String, cIGZString& destination)
	{
		bool result = false;

		if (pSC4String)
		{
			destination.Copy(*pSC4String->AsIGZString());
			result = true;
		}

		return true;
	}

	enum class BuildingFundingType
	{
		Capacity = 0,
		Coverage
	};

	bool GetBuildingFullFundingToken(
		const UnknownTokenContext* context,
		cIGZString& outReplacement,
		BuildingFundingType type)
	{
		bool result = false;

		if (context && context->pCity && context->pOccupant)
		{
			cISC4BudgetSimulator* pBudgetSim = context->pCity->GetBudgetSimulator();

			if (pBudgetSim)
			{
				eastl::vector<cISC4BudgetSimulator::BudgetItem> budgetItems;

				if (pBudgetSim->GetBudgetItemInfo(context->pOccupant->AsPropertyHolder(), budgetItems))
				{
					int64_t value = 0;

					const size_t count = budgetItems.size();

					for (size_t i = 0; i < count && value == 0; i++)
					{
						const cISC4BudgetSimulator::BudgetItem& item = budgetItems[i];

						// The built-in budget departments that use per-building variable funding
						// can have up to two have two different purpose values: Capacity and coverage.
						//
						// Capacity is used for things like the number of patients a Health building can support or
						// the coverage radius of a Fire/Police station.
						// Coverage is the coverage radius for Education and Health buildings (School Bus/Ambulance).
						if (type == BuildingFundingType::Capacity)
						{
							switch (item.purpose)
							{
							case 0xEA5654B6: // Education Staff
							case 0xEA567BC3: // Fire Protection
							case 0xCA565486: // Health Staff
							case 0x0A567BAA: // Police Protection
							case 0xCA58E540: // Power Production
								value = item.cost;
								break;
							}
						}
						else // Coverage
						{
							switch (item.purpose)
							{
							case 0x4A5654BA: // Education Coverage
							case 0xEA56549E: // Health Coverage
								value = item.cost;
								break;
							}
						}
					}

					result = MakeNumberStringForCurrentLanguage(
						value,
						outReplacement,
						NumberType::Money);
				}
			}
		}

		return result;
	}

	enum class BuildingStylesTokenSeparatorType
	{
		// A pipe character, e.g. Chicago 1890 | New York 1940.
		Pipe,
		// A new line character, each style after the first will be on its own line.
		// E.g:
		// Chicago 1890
		// New York 1940
		NewLine
	};

	std::string_view GetBuildingStylesTokenSeparator(BuildingStylesTokenSeparatorType type)
	{
		switch (type)
		{
		case BuildingStylesTokenSeparatorType::NewLine:
			return "\n"sv;
		case BuildingStylesTokenSeparatorType::Pipe:
		default:
			return " | "sv;
		}
	}

	bool GetBuildingStylesToken(
		const UnknownTokenContext* context,
		cIGZString& outReplacement,
		BuildingStylesTokenSeparatorType type)
	{
		bool result = false;

		if (context && context->pOccupant)
		{
			const std::string_view separator = GetBuildingStylesTokenSeparator(type);

			cRZAutoRefCount<cIBuildingStyleInfo2> pBuildingStyleInfo;

			if (RZGetFramework()->GetCOMObject()->GetClassObject(
				GZCLSID_cIBuildingStyleInfo,
				GZIID_cIBuildingStyleInfo2,
				pBuildingStyleInfo.AsPPVoid()))
			{
				// If the More Building Styles DLL is installed we check the building's OccupantGroups
				// for any styles that are present in the Building Style Control.
				result = pBuildingStyleInfo->GetBuildingStyleNamesEx(
					context->pOccupant,
					outReplacement,
					cRZBaseString(separator.data(), separator.size()));
			}
			else
			{
				// If the More Building Styles DLL is not installed we fall back to checking for
				// the 4 built-in Maxis styles.

				for (size_t i = 0; i < MaxisBuildingStyles.size(); i++)
				{
					const auto& item = MaxisBuildingStyles[i];

					if (context->pOccupant->IsOccupantGroup(item.first))
					{
						outReplacement.Append(item.second.data(), item.second.size());
						outReplacement.Append(separator.data(), separator.size());
					}
				}

				// Check that at least one style name has been written to the destination.
				if (outReplacement.Strlen() > 0)
				{
					// Remove the trailing separator from the last style in the list.
					outReplacement.Resize(outReplacement.Strlen() - separator.size());
					result = true;
				}
			}
		}

		return result;
	}

	bool GetBuildingSummaryToken(UnknownTokenContext* context, cIGZString& outReplacement)
	{
		bool result = false;

		if (context && context->pOccupant)
		{
			cRZAutoRefCount<cISC4BuildingOccupant> pBuildingOccupant;

			if (context->pOccupant->QueryInterface(GZIID_cISC4BuildingOccupant, pBuildingOccupant.AsPPVoid()))
			{
				const cISC4BuildingOccupant::BuildingProfile& profile = pBuildingOccupant->GetBuildingProfile();
				const cISC4BuildingOccupant::PurposeType purpose = profile.purpose;

				if (purpose != cISC4BuildingOccupant::PurposeType::None)
				{
					if (purpose == cISC4BuildingOccupant::PurposeType::Residence)
					{
						switch (profile.wealth)
						{
						case cISC4BuildingOccupant::WealthType::Low:
							result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0xCA95F27E, outReplacement);
							break;
						case cISC4BuildingOccupant::WealthType::Medium:
							result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x6A95F2B0, outReplacement);
							break;
						case cISC4BuildingOccupant::WealthType::High:
							result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0xCA95F2B8, outReplacement);
							break;
						}
					}
					else if (purpose == cISC4BuildingOccupant::PurposeType::Services)
					{
						switch (profile.wealth)
						{
						case cISC4BuildingOccupant::WealthType::Low:
							result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x8A95F2BF, outReplacement);
							break;
						case cISC4BuildingOccupant::WealthType::Medium:
							result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0xAA95F2DC, outReplacement);
							break;
						case cISC4BuildingOccupant::WealthType::High:
							result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x8A95F2E4, outReplacement);
							break;
						}
					}
					else if (purpose == cISC4BuildingOccupant::PurposeType::Office)
					{
						switch (profile.wealth)
						{
						case cISC4BuildingOccupant::WealthType::Medium:
							result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x0A95F39A, outReplacement);
							break;
						case cISC4BuildingOccupant::WealthType::High:
							result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x6A95F3B3, outReplacement);
							break;
						}
					}
					else
					{
						switch (purpose)
						{
						case cISC4BuildingOccupant::PurposeType::Agriculture:
							result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x4A95F2EB, outReplacement);
							break;
						case cISC4BuildingOccupant::PurposeType::Processing:
							result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x2A95F324, outReplacement);
							break;
						case cISC4BuildingOccupant::PurposeType::Manufacturing:
							result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x6A95F313, outReplacement);
							break;
						case cISC4BuildingOccupant::PurposeType::HighTech:
							result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0xCA95F31C, outReplacement);
							break;
						}
					}
				}
			}
		}

		return result;
	}

	bool GetBuildingWallToWallToken(UnknownTokenContext* context, cIGZString& outReplacement)
	{
		bool result = false;

		if (context && context->pOccupant)
		{
			cRZAutoRefCount<cIBuildingStyleInfo2> pBuildingStyleInfo;

			if (RZGetFramework()->GetCOMObject()->GetClassObject(
				GZCLSID_cIBuildingStyleInfo,
				GZIID_cIBuildingStyleInfo2,
				pBuildingStyleInfo.AsPPVoid()))
			{
				// If the More Building Styles DLL is installed we check the if the building is wall to wall.
				// The output string will be a localized version of Yes or No.
				if (pBuildingStyleInfo->IsWallToWall(context->pOccupant))
				{
					result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0xCA5D4F33, outReplacement);
				}
				else
				{
					result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x6A5D4F37, outReplacement);
				}
			}
			else
			{
				Logger::GetInstance().WriteLine(
					LogLevel::Error,
					"Unable to get the building W2W status. Install or update the MoreBuildingStyles DLL."
					"(https://community.simtropolis.com/files/file/36112-allow-more-building-styles-dll-plugin/).");
				outReplacement.FromChar("Unknown");
				result = true;
			}
		}

		return result;
	}

	bool GetCapacityToken(
		const UnknownTokenContext* context,
		cIGZString& outReplacement,
		cISC4BuildingDevelopmentSimulator::DeveloperType developerType)
	{
		bool result = false;

		cISC4Lot* pLot = GetOccupantLot(context);

		if (pLot)
		{
			uint16_t capacity = pLot->GetCapacity(developerType, true);
			result = MakeNumberStringForCurrentLanguage(capacity, outReplacement);
		}

		return result;
	}

	bool GetOccupancyToken(
		const UnknownTokenContext* context,
		cIGZString& outReplacement,
		cISC4BuildingDevelopmentSimulator::DeveloperType developerType)
	{
		bool result = false;

		cISC4Lot* pLot = GetOccupantLot(context);

		if (pLot)
		{
			uint16_t occupancy = pLot->GetPopulation(developerType);
			result = MakeNumberStringForCurrentLanguage(occupancy, outReplacement);
		}

		return result;
	}

	bool GetGrowthStageToken(UnknownTokenContext* context, cIGZString& outReplacement)
	{
		bool result = false;

		cISC4Lot* pLot = GetOccupantLot(context);

		if (pLot)
		{
			cISC4LotConfiguration* pLotConfiguration = pLot->GetLotConfiguration();

			if (pLotConfiguration)
			{
				uint8_t growthStage = pLotConfiguration->GetGrowthStage();
				result = MakeNumberStringForCurrentLanguage(growthStage, outReplacement);
			}
		}

		return result;
	}

	enum class JobType
	{
		LowWealth = 0,
		MediumWealth,
		HighWealth
	};

	bool GetLotJobCount(UnknownTokenContext* context, cIGZString& outReplacement, JobType type)
	{
		float jobCount = 0;

		cISC4Lot* pLot = GetOccupantLot(context);

		if (pLot)
		{
			std::array<float, 4> jobs{};

			if (pLot->GetJobs(jobs.data()))
			{
				switch (type)
				{
				case JobType::LowWealth:
					jobCount = jobs[1];
					break;
				case JobType::MediumWealth:
					jobCount = jobs[2];
					break;
				case JobType::HighWealth:
					jobCount = jobs[3];
					break;
				}
			}
		}

		return MakeNumberStringForCurrentLanguage(lroundf(jobCount), outReplacement);
	}

	bool GetLotTravelJobCount(UnknownTokenContext* context, cIGZString& outReplacement, JobType type)
	{
		uint32_t travelJobs = 0;

		cISC4Lot* pLot = GetOccupantLot(context);

		if (pLot)
		{
			// Industrial lots without their own road access will have an industrial anchor lot
			// provide road access for their workers.
			// This internal commuting is invisible to the traffic simulator, so when querying
			// an industrial anchor lot the number of workers is often higher that what that
			// lot supports on its own.
			// We report zero travel jobs for industrial lots without their own road access.
			if (pLot->GetTravelDesignate() == nullptr)
			{
				cISC4TrafficSimulator* pTrafficSim = context->pCity->GetTrafficSimulator();

				if (pTrafficSim)
				{
					switch (type)
					{
					case JobType::LowWealth:
						travelJobs = pTrafficSim->GetMaxTripCapacity(pLot->AsPropertyHolder(), 0);
						break;
					case JobType::MediumWealth:
						travelJobs = pTrafficSim->GetMaxTripCapacity(pLot->AsPropertyHolder(), 1);
						break;
					case JobType::HighWealth:
						travelJobs = pTrafficSim->GetMaxTripCapacity(pLot->AsPropertyHolder(), 2);
						break;
					}
				}
			}
		}

		return MakeNumberStringForCurrentLanguage(travelJobs, outReplacement);
	}

	bool GetMySimResidentName(UnknownTokenContext* context, cIGZString& outReplacement)
	{
		bool result = false;

		if (context && context->pOccupant && context->pCity)
		{
			cRZAutoRefCount<cISC4BuildingOccupant> pBuildingOccupant;

			if (context->pOccupant->QueryInterface(GZIID_cISC4BuildingOccupant, pBuildingOccupant.AsPPVoid()))
			{
				cISC4MySimAgentSimulator* pMySimAgentSimulator = context->pCity->GetMySimAgentSimulator();

				if (pMySimAgentSimulator)
				{
					cISC4MySim* pMySim = pMySimAgentSimulator->MySimLivesHere(pBuildingOccupant);

					if (pMySim)
					{
						cISC4Advisor* asAdvisor = pMySim->AsAdvisor();

						if (asAdvisor)
						{
							result = CopySC4StringValue(asAdvisor->GetPersonalName(), outReplacement);
						}
					}
				}
			}
		}

		return result;
	}

	bool GetResourceFilePath(const cGZPersistResourceKey& key, cRZBaseString& path)
	{
		bool result = false;

		cIGZPersistResourceManagerPtr pResMan;

		if (pResMan)
		{
			cRZAutoRefCount<cIGZPersistDBSegment> pSegment;

			if (pResMan->FindDBSegment(key, pSegment.AsPPObj()))
			{
				cRZAutoRefCount<cIGZPersistDBSegmentMultiPackedFiles> pMultiPackedFile;

				if (pSegment->QueryInterface(GZIID_cIGZPersistDBSegmentMultiPackedFiles, pMultiPackedFile.AsPPVoid()))
				{
					// cIGZPersistDBSegmentMultiPackedFiles is a collection of DAT files in a specific folder
					// and its sub-folders.
					// Call its FindDBSegment method to get the actual file.

					cRZAutoRefCount<cIGZPersistDBSegment> pMultiPackedSegment;

					if (pMultiPackedFile->FindDBSegment(key, pMultiPackedSegment.AsPPObj()))
					{
						pMultiPackedSegment->GetPath(path);
						result = true;
					}
				}
				else
				{
					pSegment->GetPath(path);
					result = true;
				}
			}
		}

		return result;
	}
}

typedef bool (*TokenDataCallback)(UnknownTokenContext*, cIGZString&);

using DeveloperType = cISC4BuildingDevelopmentSimulator::DeveloperType;

static constexpr frozen::unordered_map<frozen::string, TokenDataCallback, 38> tokenDataCallbacks =
{
	{ "building_full_funding_capacity", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetBuildingFullFundingToken(ctx, dest, BuildingFundingType::Capacity); } },
	{ "building_full_funding_coverage", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetBuildingFullFundingToken(ctx, dest, BuildingFundingType::Coverage); } },
	{ "building_is_w2w", GetBuildingWallToWallToken },
	{ "building_styles", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetBuildingStylesToken(ctx, dest, BuildingStylesTokenSeparatorType::Pipe); } },
	{ "building_style_lines", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetBuildingStylesToken(ctx, dest, BuildingStylesTokenSeparatorType::NewLine); } },
	{ "building_summary", GetBuildingSummaryToken },
	{ "growth_stage", GetGrowthStageToken },
	{ "mysim_name", GetMySimResidentName },
	{ "jobs_low_wealth", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetLotJobCount(ctx, dest, JobType::LowWealth); }},
	{ "jobs_medium_wealth", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetLotJobCount(ctx, dest, JobType::MediumWealth); }},
	{ "jobs_high_wealth", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetLotJobCount(ctx, dest, JobType::HighWealth); }},
	{ "travel_jobs_low_wealth", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetLotTravelJobCount(ctx, dest, JobType::LowWealth); }},
	{ "travel_jobs_medium_wealth", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetLotTravelJobCount(ctx, dest, JobType::MediumWealth); }},
	{ "travel_jobs_high_wealth", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetLotTravelJobCount(ctx, dest, JobType::HighWealth); }},
	{ "r1_occupancy", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetOccupancyToken(ctx, dest, DeveloperType::ResidentialLowWealth); } },
	{ "r1_capacity", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetCapacityToken(ctx, dest, DeveloperType::ResidentialLowWealth); } },
	{ "r2_occupancy", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetOccupancyToken(ctx, dest, DeveloperType::ResidentialMediumWealth); } },
	{ "r2_capacity", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetCapacityToken(ctx, dest, DeveloperType::ResidentialMediumWealth); } },
	{ "r3_occupancy", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetOccupancyToken(ctx, dest, DeveloperType::ResidentialHighWealth); } },
	{ "r3_capacity", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetCapacityToken(ctx, dest, DeveloperType::ResidentialHighWealth); } },
	{ "cs1_occupancy", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetOccupancyToken(ctx, dest, DeveloperType::CommercialServicesLowWealth); } },
	{ "cs1_capacity", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetCapacityToken(ctx, dest, DeveloperType::CommercialServicesLowWealth); } },
	{ "cs2_occupancy", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetOccupancyToken(ctx, dest, DeveloperType::CommercialServicesMediumWealth); } },
	{ "cs2_capacity", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetCapacityToken(ctx, dest, DeveloperType::CommercialServicesMediumWealth); } },
	{ "cs3_occupancy", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetOccupancyToken(ctx, dest, DeveloperType::CommercialServicesHighWealth); } },
	{ "cs3_capacity", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetCapacityToken(ctx, dest, DeveloperType::CommercialServicesHighWealth); } },
	{ "co2_occupancy", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetOccupancyToken(ctx, dest, DeveloperType::CommercialOfficeMediumWealth); } },
	{ "co2_capacity", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetCapacityToken(ctx, dest, DeveloperType::CommercialOfficeMediumWealth); } },
	{ "co3_occupancy", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetOccupancyToken(ctx, dest, DeveloperType::CommercialOfficeHighWealth); } },
	{ "co3_capacity", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetCapacityToken(ctx, dest, DeveloperType::CommercialOfficeHighWealth); } },
	{ "ir_occupancy", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetOccupancyToken(ctx, dest, DeveloperType::IndustrialAgriculture); } },
	{ "ir_capacity", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetCapacityToken(ctx, dest, DeveloperType::IndustrialAgriculture); } },
	{ "id_occupancy", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetOccupancyToken(ctx, dest, DeveloperType::IndustrialProcessing); } },
	{ "id_capacity", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetCapacityToken(ctx, dest, DeveloperType::IndustrialProcessing); } },
	{ "im_occupancy", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetOccupancyToken(ctx, dest, DeveloperType::IndustrialManufacturing); } },
	{ "im_capacity", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetCapacityToken(ctx, dest, DeveloperType::IndustrialManufacturing); } },
	{ "iht_occupancy", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetOccupancyToken(ctx, dest, DeveloperType::IndustrialHighTech); } },
	{ "iht_capacity", [](UnknownTokenContext* ctx, cIGZString& dest) { return GetCapacityToken(ctx, dest, DeveloperType::IndustrialHighTech); } },
};

static bool UnknownTokenCallback(cIGZString const& token, cIGZString& outReplacement, void* pContext)
{
	bool result = false;

	const auto& entry = tokenDataCallbacks.find(frozen::string(token.Data(), token.Strlen()));

	if (entry != tokenDataCallbacks.end())
	{
		UnknownTokenContext* context = static_cast<UnknownTokenContext*>(pContext);

		// The string may have been set to an error message by some other token callback method.
		outReplacement.Erase(0, outReplacement.Strlen());

		if (!entry->second(context, outReplacement))
		{
			// Return an empty string if the handler method failed.
			outReplacement.Erase(0, outReplacement.Strlen());
		}

		// Let the game know that the token was handled.
		result = true;
	}

	return result;
}

// The current token context is stored in a static variable to ensure that the
// memory remains valid between calls to BeforeDialogShown and AfterDialogShown.
static UnknownTokenContext sCurrentTokenContext;

BuildingQueryVariablesProvider::BuildingQueryVariablesProvider(const ISettings& settings)
	: settings(settings),
	  pStringDetokenizer(nullptr),
	  pCity(nullptr)
{
}

bool BuildingQueryVariablesProvider::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIBuildingQueryHookTarget)
	{
		*ppvObj = static_cast<cIBuildingQueryDialogHookTarget*>(this);
		AddRef();

		return true;
	}

	return DataProviderBase::QueryInterface(riid, ppvObj);
}

uint32_t BuildingQueryVariablesProvider::AddRef()
{
	return DataProviderBase::AddRef();
}

uint32_t BuildingQueryVariablesProvider::Release()
{
	return DataProviderBase::Release();
}

void BuildingQueryVariablesProvider::PostAppInit(cIGZCOM* pCOM)
{
	cISC4AppPtr pSC4App;

	if (pSC4App)
	{
		pStringDetokenizer = pSC4App->GetStringDetokenizer();
	}
}

void BuildingQueryVariablesProvider::PostCityInit(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM)
{
	pCity = static_cast<cISC4City*>(pStandardMsg->GetVoid1());

	cRZAutoRefCount<cIBuildingQueryHookServer> hookServer;

	if (pCOM->GetClassObject(
		GZCLSID_cIBuildingQueryHookServer,
		GZIID_cIBuildingQueryHookServer,
		hookServer.AsPPVoid()))
	{
		hookServer->AddNotification(this);
	}
}

void BuildingQueryVariablesProvider::PreCityShutdown(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM)
{
	pCity = nullptr;

	cRZAutoRefCount<cIBuildingQueryHookServer> hookServer;

	if (pCOM->GetClassObject(
		GZCLSID_cIBuildingQueryHookServer,
		GZIID_cIBuildingQueryHookServer,
		hookServer.AsPPVoid()))
	{
		hookServer->RemoveNotification(this);
	}
}

void BuildingQueryVariablesProvider::BeforeDialogShown(cISC4Occupant* pOccupant)
{
	if (settings.LogBuildingPluginPath())
	{
		LogBuildingOccupantPluginPath(pOccupant);
	}

	if (pStringDetokenizer)
	{
		sCurrentTokenContext.pOccupant = pOccupant;
		sCurrentTokenContext.pCity = pCity;

		pStringDetokenizer->AddUnknownTokenReplacementMethod(&UnknownTokenCallback, &sCurrentTokenContext, true);

#ifdef _DEBUG
		DebugLogTokenizerVariables();
#endif // _DEBUG
	}
}

void BuildingQueryVariablesProvider::AfterDialogShown(cISC4Occupant* pOccupant)
{
	if (pStringDetokenizer)
	{
		pStringDetokenizer->AddUnknownTokenReplacementMethod(&UnknownTokenCallback, &sCurrentTokenContext, false);
	}
}

void BuildingQueryVariablesProvider::DebugLogTokenizerVariables()
{
	for (const auto& entry : tokenDataCallbacks)
	{
		cRZBaseString token("#", 1);
		token.Append(entry.first.data(), entry.first.size());
		token.Append("#", 1);

		cRZBaseString result;

		if (pStringDetokenizer->Detokenize(token, result))
		{
			DebugUtil::PrintLineToDebugOutputFormatted("%s = %s", token.ToChar(), result.ToChar());
		}
	}
}

void BuildingQueryVariablesProvider::LogBuildingOccupantPluginPath(cISC4Occupant* pOccupant)
{
	if (pOccupant && pCity)
	{
		cRZAutoRefCount<cISC4BuildingOccupant> pBuildingOccupant;

		if (pOccupant->QueryInterface(GZIID_cISC4BuildingOccupant, pBuildingOccupant.AsPPVoid()))
		{
			cISC4BuildingDevelopmentSimulator* pBuildingDevelpmentSim = pCity->GetBuildingDevelopmentSimulator();

			if (pBuildingDevelpmentSim)
			{
				const uint32_t buildingType = pBuildingOccupant->GetBuildingType();

				cGZPersistResourceKey key;

				if (pBuildingDevelpmentSim->GetBuildingKeyFromType(buildingType, key))
				{
					cRZBaseString path;

					if (GetResourceFilePath(key, path))
					{
						Logger& logger = Logger::GetInstance();

						cRZAutoRefCount<cIGZString> userVisibleName;

						if (OccupantUtil::GetUserVisibleName(pOccupant, userVisibleName))
						{
							logger.WriteLineFormatted(
								LogLevel::Info,
								"%s (%s): %s",
								pBuildingOccupant->GetExemplarName()->ToChar(),
								userVisibleName->ToChar(),
								path.ToChar());
						}
						else
						{
							logger.WriteLineFormatted(
								LogLevel::Info,
								"%s: %s",
								pBuildingOccupant->GetExemplarName()->ToChar(),
								path.ToChar());
						}
					}
				}
			}
		}
	}
}

