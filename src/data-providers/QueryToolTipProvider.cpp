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

#include "QueryToolTipProvider.h"
#include "cIGZCOM.h"
#include "cIGZFrameWork.h"

QueryToolTipProvider::QueryToolTipProvider()
{
}

bool QueryToolTipProvider::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cINetworkQueryCustomToolTipHookTarget)
	{
		return networkToolTipHandler.QueryInterface(riid, ppvObj);
	}
	else if (riid == GZIID_cIFloraQueryCustomToolTipHookTarget)
	{
		return floraToolTipHandler.QueryInterface(riid, ppvObj);
	}
	else if (riid == GZIID_cIPropQueryCustomToolTipHookTarget)
	{
		return propToolTipHandler.QueryInterface(riid, ppvObj);
	}

	return DataProviderBase::QueryInterface(riid, ppvObj);
}

void QueryToolTipProvider::PostAppInit(cIGZCOM* pCOM)
{
	floraToolTipHandler.PostAppInit(pCOM);
	networkToolTipHandler.PostAppInit(pCOM);
	propToolTipHandler.PostAppInit(pCOM);
}

void QueryToolTipProvider::PreAppShutdown(cIGZCOM* pCOM)
{
	floraToolTipHandler.PreAppShutdown(pCOM);
	networkToolTipHandler.PreAppShutdown(pCOM);
	propToolTipHandler.PreAppShutdown(pCOM);
}

void QueryToolTipProvider::PostCityInit(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM)
{
	floraToolTipHandler.PostCityInit(pStandardMsg, pCOM);
	networkToolTipHandler.PostCityInit(pStandardMsg, pCOM);
	propToolTipHandler.PostCityInit(pStandardMsg, pCOM);
}

void QueryToolTipProvider::PreCityShutdown(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM)
{
	floraToolTipHandler.PreCityShutdown(pStandardMsg, pCOM);
	networkToolTipHandler.PreCityShutdown(pStandardMsg, pCOM);
	propToolTipHandler.PreCityShutdown(pStandardMsg, pCOM);
}
