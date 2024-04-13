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

#include "QueryToolHelpers.h"
#include <cstdint>

namespace
{
	typedef bool(__cdecl* pfnIsDebugQueryEnabled)();

	static const pfnIsDebugQueryEnabled RealIsDebugQueryEnabled = reinterpret_cast<pfnIsDebugQueryEnabled>(0x4C4700);
}

bool QueryToolHelpers::IsDebugQueryEnabled()
{
	return RealIsDebugQueryEnabled();
}

cISC4Occupant* QueryToolHelpers::GetOccupant(void* thisPtr)
{
	// The cSC4ViewInputControlQuery class has a pointer to the currently selected
	// occupant in a field at offset 0x2C.
	// We dereference the pointer at that address and cast the value to cISC4Occupant.

	const intptr_t occupantAddress = *reinterpret_cast<intptr_t*>(reinterpret_cast<intptr_t>(thisPtr) + 0x2C);
	return reinterpret_cast<cISC4Occupant*>(occupantAddress);
}
