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
#include "DataProviderBase.h"
#include "cIBuildingQueryDialogHookTarget.h"
#include "ISettings.h"

class cISC4City;
class cISCStringDetokenizer;

class BuildingQueryVariablesProvider final
	: public DataProviderBase,
	  private cIBuildingQueryDialogHookTarget
{
public:
	BuildingQueryVariablesProvider(const ISettings& settings);

	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;

	void PostAppInit(cIGZCOM* pCOM);
	void PostCityInit(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM) override;
	void PreCityShutdown(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM) override;

private:
	void BeforeDialogShown(cISC4Occupant* pOccupant) override;
	void AfterDialogShown(cISC4Occupant* pOccupant) override;

	void DebugLogTokenizerVariables();
	void DebugLogDetokenizedValue(cIGZString const& token);

	void LogBuildingOccupantPluginPath(cISC4Occupant* pOccupant);

	const ISettings& settings;
	cISC4City* pCity;
	cISCStringDetokenizer* pStringDetokenizer;
};