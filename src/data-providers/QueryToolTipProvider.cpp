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
