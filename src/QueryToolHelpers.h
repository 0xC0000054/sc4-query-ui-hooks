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

#pragma once

class cISC4Occupant;

namespace QueryToolHelpers
{
	bool IsDebugQueryEnabled();

	cISC4Occupant* GetOccupant(void* thisPtr);
}