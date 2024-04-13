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

static const uint32_t GZIID_cIQueryToolTipAppendTextHookTarget = 0x11C834C3;

class cIQueryToolTipAppendTextHookTarget : public cIGZUnknown
{
public:
	/**
	 * @brief A callback that allows text to be appended at the bottom of a query hover tool tip.
	 * @param occupant The occupant that is being queried.
	 * @param debugQuery true if the advanced/debug query mode is active; otherwise, false.
	 * @param textToAppend The text that should be appended.
	 * @return true if text should be appended; otherwise, false.
	 */
	virtual bool AppendQueryToolTipText(
		cISC4Occupant* const occupant,
		bool debugQuery,
		cIGZString& textToAppend) = 0;
};