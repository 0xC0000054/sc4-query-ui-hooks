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

static const uint32_t GZIID_cINetworkQueryToolTipHookTarget = 0x82A5EDA1;

class cINetworkQueryCustomToolTipHookTarget : public cIGZUnknown
{
public:
	/**
	 * @brief A callback that allows for customized network query hover tool tips.
	 * @param occupant The occupant that is being queried.
	 * @param debugQuery true if the advanced/debug query mode is active; otherwise, false.
	 * @param title The title of the tool tip.
	 * @param text The tool tip message.
	 * @return true if a custom tool tip was set; otherwise, false to show the game's default tool tip.
	 */
	virtual bool ProcessToolTip(
		cISC4Occupant* const occupant,
		bool debugQuery,
		cIGZString& title,
		cIGZString& text) = 0;
};