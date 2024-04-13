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

class cIGZString;
class cISC4Occupant;

static const uint32_t GZIID_cIBuildingQueryCustomToolTipHookTarget = 0xB029E0D8;

class cIBuildingQueryCustomToolTipHookTarget : public cIGZUnknown
{
public:
	/**
	 * @brief A callback that allows for custom building query hover tool tips.
	 * @param occupant The occupant that is being queried.
	 * @param debugQuery true if the advanced/debug query mode is active; otherwise, false.
	 * @param title The title of the tool tip.
	 * @param text The tool tip message.
	 * @param backgroundImageIID The instance ID of the tool tip background image, 0 for none.
	 * @param meterImageIID The instance ID of the tool tip meter (efficiency graph) image, 0 for none.
	 * @param meterPercentage The meter percentage, in the range of [0.0-1.0].
	 * @return true if a custom tool tip was set; otherwise, false to show the game's default tool tip.
	 */
	virtual bool ProcessToolTip(
		cISC4Occupant* const occupant,
		bool debugQuery,
		cIGZString& title,
		cIGZString& text,
		uint32_t& backgroundImageIID,
		uint32_t& meterImageIID,
		float& meterPercentage) = 0;
};