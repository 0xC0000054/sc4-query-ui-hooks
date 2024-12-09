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
#include "DataProviderBase.h"
#include "cIGZMessage2Standard.h"
#include "FloraQueryToolTipHandler.h"
#include "NetworkQueryToolTipHandler.h"
#include "PropQueryToolTipHandler.h"

class cIGZString;

class QueryToolTipProvider final : public DataProviderBase
{
public:
	QueryToolTipProvider();

	bool QueryInterface(uint32_t riid, void** ppvObj) override;

	void PostAppInit(cIGZCOM* pCOM) override;
	void PreAppShutdown(cIGZCOM* pCOM) override;

	void PostCityInit(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM);
	void PreCityShutdown(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM);

private:
	FloraQueryToolTipHandler floraToolTipHandler;
	NetworkQueryToolTipHandler networkToolTipHandler;
	PropQueryToolTipHandler propToolTipHandler;
};

