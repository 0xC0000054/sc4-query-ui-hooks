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
#include "cINetworkQueryCustomToolTipHookTarget.h"
#include "cIGZMessage2Standard.h"
#include "cISC4NetworkOccupant.h"
#include "cRZMessage2COMDirector.h"

class cIGZString;
class cISC4PlumbingSimulator;

class NetworkQueryToolTipDllDirector final
	: public cRZMessage2COMDirector,
	  private cINetworkQueryCustomToolTipHookTarget
{
public:

	NetworkQueryToolTipDllDirector();

	bool QueryInterface(uint32_t riid, void** ppvObj) override;

	uint32_t AddRef() override;

	uint32_t Release() override;

	uint32_t GetDirectorID() const override;

	bool OnStart(cIGZCOM* pCOM) override;

	bool PostAppInit() override;

	bool DoMessage(cIGZMessage2* pMsg) override;

private:

	void PostCityInit(cIGZMessage2Standard* pStandardMsg);
	void PreCityShutdown(cIGZMessage2Standard* pStandardMsg);

	void SetDebugQueryText(
		cISC4NetworkOccupant* pNetworkOccupant,
		cISC4NetworkOccupant::eNetworkType networkType,
		cIGZString& text);

	bool ProcessToolTip(
		cISC4Occupant* const occupant,
		bool debugQuery,
		cIGZString& title,
		cIGZString& text) override;

	cISC4PlumbingSimulator* pPlumbingSim;
};

