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

#pragma once
#include "cSC4BaseOccupantFilter.h"

class ClickToCopyOccupantFilter final : public cSC4BaseOccupantFilter
{
public:
	ClickToCopyOccupantFilter();

	// cISC4OccupantFilter

	bool IsOccupantTypeIncluded(uint32_t type) override;
	bool IsPropertyHolderIncluded(cISCPropertyHolder* pProperties) override;
};

