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
#include "QueryToolTipHandlerBase.h"
#include "cINetworkQueryCustomToolTipHookTarget.h"
#include "cIGZCOM.h"
#include "cIGZMessage2Standard.h"
#include "cISC4NetworkOccupant.h"
#include "cISC4PlumbingSimulator.h"

class NetworkQueryToolTipHandler :
	public QueryToolTipHandlerBase,
	private cINetworkQueryCustomToolTipHookTarget
{
public:
	NetworkQueryToolTipHandler();

	// QueryToolTipHandlerBase

	void PostCityInit(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM) override;
	void PreCityShutdown(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM) override;

	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;

private:
	// cINetworkQueryCustomToolTipHookTarget

	bool ProcessToolTip(
		cISC4Occupant* const occupant,
		bool debugQuery,
		cIGZString& title,
		cIGZString& text) override;

	// Private members

	void SetDebugQueryText(
		cISC4NetworkOccupant* pNetworkOccupant,
		cISC4NetworkOccupant::eNetworkType networkType,
		cIGZString& text);

	cISC4PlumbingSimulator* pPlumbingSim;
};

