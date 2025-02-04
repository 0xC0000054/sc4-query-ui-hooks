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

#pragma once
#include "cIGZString.h"
#include "cRZAutoRefCount.h"

class cISC4Occupant;

namespace OccupantUtil
{
	bool GetDisplayName(cISC4Occupant* pOccupant, cIGZString& name);
	bool GetExemplarName(cISC4Occupant* pOccupant, cIGZString& name);
	bool GetUserVisibleName(cISC4Occupant* pOccupant, cRZAutoRefCount<cIGZString>& name);
}