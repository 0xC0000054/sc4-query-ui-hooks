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

#include "OccupantUtil.h"
#include "cISCPropertyHolder.h"
#include "cISC4Occupant.h"
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

bool OccupantUtil::GetUserVisibleName(cISC4Occupant* pOccupant, cRZAutoRefCount<cIGZString>& name)
{
	bool result = false;

	if (pOccupant)
	{
		result = PropertyUtil::GetUserVisibleName(pOccupant->AsPropertyHolder(), name);
	}

	return result;
}
