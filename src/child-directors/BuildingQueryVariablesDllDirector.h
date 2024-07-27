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
#include "cIBuildingQueryDialogHookTarget.h"
#include "cRZMessage2COMDirector.h"

class cIGZMessage2Standard;
class cISC4City;
class cISCStringDetokenizer;

class BuildingQueryVariablesDllDirector final
	: public cRZMessage2COMDirector,
	  private cIBuildingQueryDialogHookTarget
{
public:

	BuildingQueryVariablesDllDirector();

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

	void BeforeDialogShown(cISC4Occupant* pOccupant) override;
	void AfterDialogShown(cISC4Occupant* pOccupant) override;

	void DebugLogTokenizerVariables();

	cISC4City* pCity;;
	cISCStringDetokenizer* pStringDetokenizer;
};