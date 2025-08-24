/*
 * This file is part of sc4-query-ui-hooks, a DLL Plugin for SimCity 4 that
 * extends the query UI.
 *
 * Copyright (C) 2024, 2025 Nicholas Hayes
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

#include "ClickToCopyOccupantFilter.h"

ClickToCopyOccupantFilter::ClickToCopyOccupantFilter()
{
}

bool ClickToCopyOccupantFilter::IsOccupantTypeIncluded(uint32_t type)
{
	switch (type)
	{
	case 0x278128a0: // Building
	case 0x74758926: // Flora
	case 0xa823821e: // Prop
		return true;
	default:
		return false;
	}
}

bool ClickToCopyOccupantFilter::IsPropertyHolderIncluded(cISCPropertyHolder* pProperties)
{
	return false;
}
