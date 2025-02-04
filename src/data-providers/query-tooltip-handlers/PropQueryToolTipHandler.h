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
#include "QueryToolTipHandlerBase.h"
#include "cIPropQueryCustomToolTipHookTarget.h"
#include "cIGZCOM.h"
#include "cIGZDate.h"
#include "cIGZLanguageManager.h"
#include "cIGZMessage2Standard.h"
#include "cRZBaseString.h"

class PropQueryToolTipHandler final :
	public QueryToolTipHandlerBase,
	private cIPropQueryCustomToolTipHookTarget
{
public:
	PropQueryToolTipHandler();

	// QueryToolTipHandlerBase

	void PostCityInit(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM) override;
	void PreCityShutdown(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM) override;

	// cIGZUnknown

	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;

private:
	// cIPropQueryCustomToolTipHookTarget

	bool ProcessToolTip(
		cISC4Occupant* const occupant,
		bool debugQuery,
		cIGZString& title,
		cIGZString& text) override;

	// Private members
};

