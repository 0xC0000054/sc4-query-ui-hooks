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

#include "QueryToolHelpers.h"
#include "Windows.h"
#include <cstdint>

namespace
{
	bool IsKeyDownNow(int32_t vKey)
	{
		return (GetAsyncKeyState(vKey) & 0x8000) != 0;
	}
}

bool QueryToolHelpers::IsDebugQueryEnabled()
{
	// Check for the Control + Alt + Shift key combination.
	// We don't call the method that the Maxis query dialog uses because it only
	// checks for the key combination if the game's debug functionality is enabled.
	return IsKeyDownNow(VK_CONTROL) && IsKeyDownNow(VK_MENU) && IsKeyDownNow(VK_SHIFT);
}

cISC4Occupant* QueryToolHelpers::GetOccupant(void* thisPtr)
{
	// The cSC4ViewInputControlQuery class has a pointer to the currently selected
	// occupant in a field at offset 0x2C.
	// We dereference the pointer at that address and cast the value to cISC4Occupant.

	const intptr_t occupantAddress = *reinterpret_cast<intptr_t*>(reinterpret_cast<intptr_t>(thisPtr) + 0x2C);
	return reinterpret_cast<cISC4Occupant*>(occupantAddress);
}
