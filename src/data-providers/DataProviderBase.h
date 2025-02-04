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
#include "cIGZCOM.h"
#include "cIGZMessage2Standard.h"

class DataProviderBase : public cIGZUnknown
{
public:
	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;

	virtual void PostAppInit(cIGZCOM* pCOM);
	virtual void PreAppShutdown(cIGZCOM* pCOM);

	virtual void PostCityInit(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM) = 0;
	virtual void PreCityShutdown(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM) = 0;
protected:
	DataProviderBase();
private:
	uint32_t refCount;
};

