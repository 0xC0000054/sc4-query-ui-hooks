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

class cISC4Occupant;

namespace DebugUtil
{
	void PrintLineToDebugOutput(const char* const line);
	void PrintLineToDebugOutputFormatted(const char* const format, ...);

	void PrintOccupantNameToDebugOutput(cISC4Occupant* pOccupant);
}