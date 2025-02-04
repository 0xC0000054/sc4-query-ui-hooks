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
