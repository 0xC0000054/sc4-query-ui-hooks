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
#include "cIGZMessage2Standard.h"
#include "cRZMessage2COMDirector.h"
#include "FloraQueryToolTipHandler.h"
#include "NetworkQueryToolTipHandler.h"

class cIGZString;

class QueryToolTipDllDirector final : public cRZMessage2COMDirector
{
public:

	QueryToolTipDllDirector();

	bool QueryInterface(uint32_t riid, void** ppvObj) override;

	uint32_t GetDirectorID() const override;

	bool OnStart(cIGZCOM* pCOM) override;

private:

	bool PostAppInit() override;
	bool PreAppShutdown() override;

	bool DoMessage(cIGZMessage2* pMsg) override;


	void PostCityInit(cIGZMessage2Standard* pStandardMsg);
	void PreCityShutdown(cIGZMessage2Standard* pStandardMsg);

	FloraQueryToolTipHandler floraToolTipHandler;
	NetworkQueryToolTipHandler networkToolTipHandler;
};

