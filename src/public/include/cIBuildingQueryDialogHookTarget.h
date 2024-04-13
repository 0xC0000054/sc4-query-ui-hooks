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
#include "cIGZUnknown.h"

class cISC4Occupant;

static const uint32_t GZIID_cIBuildingQueryHookTarget = 0x47CB9342;

class cIBuildingQueryDialogHookTarget : public cIGZUnknown
{
public:
	/**
	 * @brief This callback is executed before the game shows the building query dialog.
	 * @param occupant The occupant that was queried.
	 */
	virtual void BeforeDialogShown(cISC4Occupant* occupant) = 0;

	/**
	 * @brief This callback is executed after the game shows the building query dialog.
	 * @param occupant The occupant that was queried.
	 */
	virtual void AfterDialogShown(cISC4Occupant* occupant) = 0;
};
