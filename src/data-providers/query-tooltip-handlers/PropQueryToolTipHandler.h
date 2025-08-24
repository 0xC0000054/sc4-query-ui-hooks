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

