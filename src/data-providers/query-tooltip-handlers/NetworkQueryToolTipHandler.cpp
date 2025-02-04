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

#include "NetworkQueryToolTipHandler.h"
#include "cINetworkQueryToolTipHookServer.h"
#include "GZStringUtil.h"
#include "Logger.h"
#include "cIGZApp.h"
#include "cIGZCOM.h"
#include "cIGZFrameWork.h"
#include "cIGZMessage2Standard.h"
#include "cIGZString.h"
#include "cISC4City.h"
#include "cISC4NetworkOccupant.h"
#include "cISC4Occupant.h"
#include "cISC4PlumbingSimulator.h"
#include "cSC4EdgeConnectionStore.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseString.h"
#include <GZServPtrs.h>

#include <array>
#include <vector>

using namespace std::string_view_literals;

static constexpr std::pair<cISC4NetworkOccupant::eNetworkType, uint32_t> MakeListEntry(cISC4NetworkOccupant::eNetworkType type)
{
	// The lower 12 bits of the network flags identify the network type,
	// so we set the appropriate flag for each type.
	// This optimization allows us to avoid having to call cISC4NetworkOccupant::IsOfType for each item.
	// It also simplifies detecting networks that use multiple types.
	return std::make_pair(type, 1U << static_cast<uint32_t>(type));
}

// A list of the network types in the order that SC4's query tool checks for them.
static const std::array<std::pair<cISC4NetworkOccupant::eNetworkType, uint32_t>, 13> NetworkTypeList =
{
	MakeListEntry(cISC4NetworkOccupant::Highway),       // 2
	MakeListEntry(cISC4NetworkOccupant::LightRail),     // 8
	MakeListEntry(cISC4NetworkOccupant::GroundHighway), // 0xC
	MakeListEntry(cISC4NetworkOccupant::Road),          // 0
	MakeListEntry(cISC4NetworkOccupant::Rail),          // 1
	MakeListEntry(cISC4NetworkOccupant::OneWayRoad),    // 0xA
	MakeListEntry(cISC4NetworkOccupant::DirtRoad),      // 0xB
	MakeListEntry(cISC4NetworkOccupant::Monorail),      // 9
	MakeListEntry(cISC4NetworkOccupant::Street),        // 3
	MakeListEntry(cISC4NetworkOccupant::WaterPipe),     // 4
	MakeListEntry(cISC4NetworkOccupant::Subway),        // 7
	MakeListEntry(cISC4NetworkOccupant::Avenue),        // 6
	// This value doesn't appear in the cSC4ViewInputControlQuery::GetNetworkOccupantSummaryInfo
	// list, but we include it for completeness.
	MakeListEntry(cISC4NetworkOccupant::PowerPole),     // 5
};

namespace
{
	bool GetNetworkType(cISC4NetworkOccupant* const networkOccupant, cISC4NetworkOccupant::eNetworkType& type)
	{
		bool result = false;

		if (networkOccupant)
		{
			const uint32_t networkFlags = networkOccupant->GetNetworkFlag();

			for (const auto& item : NetworkTypeList)
			{
				if ((networkFlags & item.second) != 0)
				{
					type = item.first;
					result = true;
					break;
				}
			}
		}

		return result;
	}

	bool SetNetworkTitle(cISC4NetworkOccupant::eNetworkType networkType, cIGZString& title)
	{
		bool result = false;

		switch (networkType)
		{
		case cISC4NetworkOccupant::Road:
			result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x6A5589D4, title);
			break;
		case cISC4NetworkOccupant::Rail:
			result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x4A5589F3, title);
			break;
		case cISC4NetworkOccupant::Highway:
			result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0xAA5589FB, title);
			break;
		case cISC4NetworkOccupant::Street:
			result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x8A558A04, title);
			break;
		case cISC4NetworkOccupant::WaterPipe:
			result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x0A558A0A, title);
			break;
		case cISC4NetworkOccupant::PowerPole:
			result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x0A5BB9FD, title);
			break;
		case cISC4NetworkOccupant::Avenue:
			result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x8B8C5CD0, title);
			break;
		case cISC4NetworkOccupant::Subway:
			result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x8A558A10, title);
			break;
		case cISC4NetworkOccupant::LightRail:
			result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x8B8C5CBD, title);
			break;
		case cISC4NetworkOccupant::Monorail:
			result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x0BE09CA2, title);
			break;
		case cISC4NetworkOccupant::OneWayRoad:
			result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x4BD9FDD8, title);
			break;
		case cISC4NetworkOccupant::DirtRoad:
			result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0xCBE09C9B, title);
			break;
		case cISC4NetworkOccupant::GroundHighway:
			result = GZStringUtil::SetLocalizedStringValue(0xEA5524EB, 0x2BE09C9E, title);
			break;
		}

		return result;
	}

	std::string GetNetworkEnglishName(cISC4NetworkOccupant::eNetworkType networkType)
	{
		std::string result;

		switch (networkType)
		{
		case cISC4NetworkOccupant::Road:
			result = "Road";
			break;
		case cISC4NetworkOccupant::Rail:
			result = "Rail";
			break;
		case cISC4NetworkOccupant::Highway:
			result = "Highway";
			break;
		case cISC4NetworkOccupant::Street:
			result = "Street";
			break;
		case cISC4NetworkOccupant::WaterPipe:
			result = "Pipe";
			break;
		case cISC4NetworkOccupant::PowerPole:
			result = "PowerPole";
			break;
		case cISC4NetworkOccupant::Avenue:
			result = "Avenue";
			break;
		case cISC4NetworkOccupant::Subway:
			result = "Subway";
			break;
		case cISC4NetworkOccupant::LightRail:
			result = "LightRail";
			break;
		case cISC4NetworkOccupant::Monorail:
			result = "Monorail";
			break;
		case cISC4NetworkOccupant::OneWayRoad:
			result = "OneWayRaod";
			break;
		case cISC4NetworkOccupant::DirtRoad:
			result = "DirtRoad";
			break;
		case cISC4NetworkOccupant::GroundHighway:
			result = "GroundHighway";
			break;
		}

		return result;
	}

	std::string GetNetworkTypesString(cISC4NetworkOccupant* const networkOccupant)
	{
		std::string networkTypes("Network Types: ");

		const uint32_t networkFlags = networkOccupant->GetNetworkFlag();

		bool firstItem = true;

		for (const auto& item : NetworkTypeList)
		{
			if ((networkFlags & item.second) != 0)
			{
				std::string name = GetNetworkEnglishName(item.first);

				if (!firstItem)
				{
					networkTypes.append(1, '/');
				}

				networkTypes.append(name);
				firstItem = false;
			}
		}

		networkTypes.append(1, '\n');

		return networkTypes;
	}

	std::string GetNetworkOrientation(cISC4NetworkOccupant* const networkOccupant)
	{
		std::string orientation("Network Orientation: ");

		const uint8_t value = networkOccupant->GetRotationAndFlip();

		switch (value)
		{
		case 0x0:
			orientation.append("North = 0,0\n");
			break;
		case 0x1:
			orientation.append("East = 1,0\n");;
			break;
		case 0x2:
			orientation.append("South = 2,0\n");
			break;
		case 0x3:
			orientation.append("West = 3,0\n");
			break;
		case 0x80:
			orientation.append("North, mirrored = 0,1\n");
			break;
		case 0x81:
			orientation.append("East, mirrored = 1,1\n");
			break;
		case 0x82:
			orientation.append("South, mirrored = 2,1\n");
			break;
		case 0x83:
			orientation.append("West, mirrored = 3,1\n");
			break;
		}

		return orientation;
	}

	void AppendEdgeConnectionData(int networkNumber, const EdgeConnection& data, cIGZString& destination)
	{
		GZStringUtil::AppendLineFormatted(
			destination,
			"  Network %d (%s)",
			networkNumber,
			GetNetworkEnglishName(static_cast<cISC4NetworkOccupant::eNetworkType>(data.networkType & 0xFF)).c_str());

		// The game stores the edge connection types using octal notation, so we print them using that format.
		GZStringUtil::AppendLineFormatted(
			destination,
			"    WNES = %02o,%02o,%02o,%02o",
			data.edgeData.directions.west,
			data.edgeData.directions.north,
			data.edgeData.directions.east,
			data.edgeData.directions.south);
	}

	void AppendEdgeConnections(cISC4NetworkOccupant* const networkOccupant, cIGZString& destination)
	{
		const cSC4EdgeConnectionStore* edgeConnectionStore = networkOccupant->GetEdgeStore();

		if (edgeConnectionStore)
		{
			GZStringUtil::AppendLine("Edge Connections:"sv, destination);
			AppendEdgeConnectionData(1, edgeConnectionStore->firstNetwork, destination);

			// The game only uses the lowest byte, so the upper 3 bytes can be garbage.
			const uint8_t additionalNetworkCount = edgeConnectionStore->additionalNetworkCount & 0xFF;

			if (additionalNetworkCount > 0)
			{
				if (edgeConnectionStore->additionalNetworkArray)
				{
					for (int i = 0; i < additionalNetworkCount; i++)
					{
						const EdgeConnection& data = edgeConnectionStore->additionalNetworkArray[i];

						AppendEdgeConnectionData(2 + i, data, destination);
					}
				}
				else
				{
					if (additionalNetworkCount == 1)
					{
						// When there are only 2 edge networks and both networks are the same type (e.g. Road/Road),
						// the game will not allocate the additional network array if the second network has edge
						// connection values that are all 0.
						// This optimization would allow the game to save 8 bytes of memory in that case.

						EdgeConnection data{};
						data.networkType = edgeConnectionStore->firstNetwork.networkType & 0xFF;
						data.edgeData.packedEdgeData = 0;

						AppendEdgeConnectionData(2, data, destination);
					}
				}
			}
		}
	}

	struct cSC4NetworkOccupant
	{
		void* vtable;
		uint8_t unknown1[252];
		uint32_t underTextureID;
	};

	static_assert(offsetof(cSC4NetworkOccupant, underTextureID) == 0x100);

	uint32_t GetUnderTextureID(const cISC4NetworkOccupant* pNetworkOccupant)
	{
		return reinterpret_cast<const cSC4NetworkOccupant*>(pNetworkOccupant)->underTextureID;
	}
}

NetworkQueryToolTipHandler::NetworkQueryToolTipHandler()
	: pPlumbingSim(nullptr)
{
}

void NetworkQueryToolTipHandler::PostCityInit(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM)
{
	cISC4City* pCity = static_cast<cISC4City*>(pStandardMsg->GetVoid1());

	pPlumbingSim = pCity->GetPlumbingSimulator();

	cRZAutoRefCount<cINetworkQueryToolTipHookServer> hookServer;

	if (pCOM->GetClassObject(
		GZCLSID_cINetworkQueryToolTipHookServer,
		GZIID_cINetworkQueryToolTipHookServer,
		hookServer.AsPPVoid()))
	{
		hookServer->AddNotification(this);
	}
}

void NetworkQueryToolTipHandler::PreCityShutdown(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM)
{
	pPlumbingSim = nullptr;

	cRZAutoRefCount<cINetworkQueryToolTipHookServer> hookServer;

	if (pCOM->GetClassObject(
		GZCLSID_cINetworkQueryToolTipHookServer,
		GZIID_cINetworkQueryToolTipHookServer,
		hookServer.AsPPVoid()))
	{
		hookServer->RemoveNotification(this);
	}
}

bool NetworkQueryToolTipHandler::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cINetworkQueryCustomToolTipHookTarget)
	{
		*ppvObj = static_cast<cINetworkQueryCustomToolTipHookTarget*>(this);
		AddRef();

		return true;
	}

	return QueryToolTipHandlerBase::QueryInterface(riid, ppvObj);
}

uint32_t NetworkQueryToolTipHandler::AddRef()
{
	return QueryToolTipHandlerBase::AddRef();
}

uint32_t NetworkQueryToolTipHandler::Release()
{
	return QueryToolTipHandlerBase::Release();
}

bool NetworkQueryToolTipHandler::ProcessToolTip(cISC4Occupant* const occupant, bool debugQuery, cIGZString& title, cIGZString& text)
{
	bool result = false;

	if (debugQuery && occupant)
	{
		cRZAutoRefCount<cISC4NetworkOccupant> networkOccupant;

		if (occupant->QueryInterface(GZIID_cISC4NetworkOccupant, networkOccupant.AsPPVoid()))
		{
			cISC4NetworkOccupant::eNetworkType networkType{};

			if (GetNetworkType(networkOccupant, networkType))
			{
				result = SetNetworkTitle(networkType, title);

				if (result)
				{
					SetDebugQueryText(networkOccupant, networkType, text);
				}
			}
		}
	}

	return result;
}

void NetworkQueryToolTipHandler::SetDebugQueryText(
	cISC4NetworkOccupant* pNetworkOccupant,
	cISC4NetworkOccupant::eNetworkType networkType,
	cIGZString& text)
{
	if (networkType == cISC4NetworkOccupant::WaterPipe)
	{
		if (pPlumbingSim)
		{
			uint32_t numPipes = pPlumbingSim->GetNumPipes();
			uint32_t numDistressedPipes = pPlumbingSim->GetNumDistressedPipes();

			text.Sprintf("Distressed Pipes: %d / %d\n", numDistressedPipes, numPipes);
		}
	}
	else if (networkType != cISC4NetworkOccupant::PowerPole)
	{
		GZStringUtil::AppendLine(GetNetworkTypesString(pNetworkOccupant), text);
		GZStringUtil::AppendLineFormatted(text, "Network Piece ID: 0x%08x\n", pNetworkOccupant->PieceId());
		GZStringUtil::AppendLineFormatted(text, "Network Piece Base Texture: 0x%08x\n", GetUnderTextureID(pNetworkOccupant));
		GZStringUtil::AppendLineFormatted(text, "Network Piece Wealth: %d\n", pNetworkOccupant->GetVariation());
		GZStringUtil::AppendLine(GetNetworkOrientation(pNetworkOccupant), text);

		uint32_t x = 0;
		uint32_t z = 0;

		pNetworkOccupant->GetOccupiedCell(x, z);

		GZStringUtil::AppendLineFormatted(text, "Cell: x = %d, z = %d", x, z);
		AppendEdgeConnections(pNetworkOccupant, text);
	}
}
