/*
 * This file is part of sc4-query-ui-hooks, a DLL Plugin for SimCity 4 that
 * extends the query UI.
 *
 * Copyright (C) 2024, 2025, 2026 Nicholas Hayes
 *
 * sc4-query-ui-hooks is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * sc4-query-ui-hooks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with sc4-query-ui-hooks.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "BuildingPluginInfo.h"
#include "cGZPersistResourceKey.h"
#include "cIGZPersistDBSegment.h"
#include "cIGZPersistDBSegmentMultiPackedFiles.h"
#include "cIGZPersistResourceManager.h"
#include "cISC4BuildingDevelopmentSimulator.h"
#include "cISC4City.h"
#include "cISC4Lot.h"
#include "cISC4LotConfiguration.h"
#include "cRZAutoRefCount.h"
#include "cRZBaseString.h"
#include "GlobalSC4InterfacePointers.h"
#include "GZServPtrs.h"
#include "Logger.h"
#include "OccupantUtil.h"

namespace
{
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

	bool GetBuildingExemplarFilePath(uint32_t buildingType, cRZBaseString& path)
	{
		bool result = false;

		cISC4BuildingDevelopmentSimulator* pBuildingDevelpmentSim = spCity->GetBuildingDevelopmentSimulator();

		if (pBuildingDevelpmentSim)
		{
			cGZPersistResourceKey key;

			if (pBuildingDevelpmentSim->GetBuildingKeyFromType(buildingType, key))
			{
				result = GetResourceFilePath(key, path);
			}
		}

		return result;
	}

	bool GetLotExemplarFilePath(uint32_t lotID, cRZBaseString& path)
	{
		// SC4 requires lot configuration exemplars to use the 0xa8fbd372 group id.
		cGZPersistResourceKey key(0x6534284a, 0xa8fbd372, lotID);

		return GetResourceFilePath(key, path);
	}
}

void BuildingPluginInfo::WriteToLog(cISC4Occupant* pOccupant)
{
	cISC4Lot* pLot = OccupantUtil::GetLot(pOccupant);

	if (pLot)
	{
		cISC4LotConfiguration* pLotConfiguration = pLot->GetLotConfiguration();

		if (pLotConfiguration)
		{
			Logger& logger = Logger::GetInstance();

			cISC4BuildingOccupant* pBuilding = pLot->GetBuilding();

			if (pBuilding)
			{
				logger.WriteLineFormatted(LogLevel::Info, "%s:", pBuilding->GetBuildingName()->ToChar());

				const uint32_t buildingType = pBuilding->GetBuildingType();

				cRZBaseString buildingExemplarFilePath;

				if (GetBuildingExemplarFilePath(buildingType, buildingExemplarFilePath))
				{
					logger.WriteLineFormatted(
						LogLevel::Info,
						"    Building: 0x%08x (%s): %s",
						buildingType,
						pBuilding->GetExemplarName()->ToChar(),
						buildingExemplarFilePath.ToChar());
				}
				else
				{
					logger.WriteLineFormatted(
						LogLevel::Info,
						"    Building: 0x%08x (%s)",
						buildingType,
						pBuilding->GetExemplarName()->ToChar());
				}

				const uint32_t lotID = pLotConfiguration->GetID();

				cRZBaseString lotName;
				pLotConfiguration->GetName(lotName);

				cRZBaseString lotExemplarFilePath;

				if (GetLotExemplarFilePath(lotID, lotExemplarFilePath))
				{
					logger.WriteLineFormatted(
						LogLevel::Info,
						"    Lot: 0x%08X (%s): %s",
						lotID,
						lotName.ToChar(),
						lotExemplarFilePath.ToChar());
				}
				else
				{
					logger.WriteLineFormatted(
						LogLevel::Info,
						"    Lot: 0x%08X (%s)",
						lotID,
						lotName.ToChar());
				}
			}
			else
			{
				const uint32_t lotID = pLotConfiguration->GetID();

				cRZBaseString lotName;
				pLotConfiguration->GetName(lotName);

				cRZBaseString lotExemplarFilePath;

				if (GetLotExemplarFilePath(lotID, lotExemplarFilePath))
				{
					logger.WriteLineFormatted(
						LogLevel::Info,
						"Lot: 0x%08X (%s): %s",
						lotID,
						lotName.ToChar(),
						lotExemplarFilePath.ToChar());
				}
				else
				{
					logger.WriteLineFormatted(
						LogLevel::Info,
						"Lot: 0x%08X (%s)",
						lotID,
						lotName.ToChar());
				}
			}
		}
	}
}
