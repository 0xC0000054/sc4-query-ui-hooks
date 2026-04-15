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

#include "OccupantUtil.h"
#include "cISCPropertyHolder.h"
#include "cISC4City.h"
#include "cISC4LotManager.h"
#include "cISC4Occupant.h"
#include "GlobalSC4InterfacePointers.h"
#include "PropertyUtil.h"

bool OccupantUtil::GetDisplayName(cISC4Occupant* pOccupant, cIGZString& name)
{
	bool result = false;

	if (pOccupant)
	{
		result = PropertyUtil::GetDisplayName(pOccupant->AsPropertyHolder(), name);
	}

	return result;
}

bool OccupantUtil::GetExemplarName(cISC4Occupant* pOccupant, cIGZString& name)
{
	bool result = false;

	if (pOccupant)
	{
		result = PropertyUtil::GetExemplarName(pOccupant->AsPropertyHolder(), name);
	}

	return result;
}

cISC4Lot* OccupantUtil::GetLot(cISC4Occupant* pOccupant)
{
	cISC4Lot* pLot = nullptr;

	if (pOccupant && spCity)
	{
		cISC4LotManager* pLotManager = spCity->GetLotManager();

		if (pLotManager)
		{
			pLot = pLotManager->GetOccupantLot(pOccupant);
		}
	}

	return pLot;
}

bool OccupantUtil::GetUserVisibleName(cISC4Occupant* pOccupant, cRZAutoRefCount<cIGZString>& name)
{
	bool result = false;

	if (pOccupant)
	{
		result = PropertyUtil::GetUserVisibleName(pOccupant->AsPropertyHolder(), name);
	}

	return result;
}
