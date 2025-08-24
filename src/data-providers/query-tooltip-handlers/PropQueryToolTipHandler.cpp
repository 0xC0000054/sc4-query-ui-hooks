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

#include "PropQueryToolTipHandler.h"
#include "cIPropQueryToolTipHookServer.h"
#include "cIGZLanguageUtility.h"
#include "cISC4City.h"
#include "cISC4Occupant.h"
#include "cISC4Simulator.h"
#include "GZStringUtil.h"
#include "OccupantUtil.h"
#include "GZServPtrs.h"

PropQueryToolTipHandler::PropQueryToolTipHandler()
{
}

void PropQueryToolTipHandler::PostCityInit(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM)
{
	cRZAutoRefCount<cIPropQueryToolTipHookServer> hookServer;

	if (pCOM->GetClassObject(
		GZCLSID_cIPropQueryToolTipHookServer,
		GZIID_cIPropQueryToolTipHookServer,
		hookServer.AsPPVoid()))
	{
		hookServer->AddNotification(this);
	}
}

void PropQueryToolTipHandler::PreCityShutdown(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM)
{
	cRZAutoRefCount<cIPropQueryToolTipHookServer> hookServer;

	if (pCOM->GetClassObject(
		GZCLSID_cIPropQueryToolTipHookServer,
		GZIID_cIPropQueryToolTipHookServer,
		hookServer.AsPPVoid()))
	{
		hookServer->RemoveNotification(this);
	}
}

bool PropQueryToolTipHandler::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIPropQueryCustomToolTipHookTarget)
	{
		*ppvObj = static_cast<cIPropQueryCustomToolTipHookTarget*>(this);
		AddRef();

		return true;
	}

	return QueryToolTipHandlerBase::QueryInterface(riid, ppvObj);
}

uint32_t PropQueryToolTipHandler::AddRef()
{
	return QueryToolTipHandlerBase::AddRef();
}

uint32_t PropQueryToolTipHandler::Release()
{
	return QueryToolTipHandlerBase::Release();
}

bool PropQueryToolTipHandler::ProcessToolTip(
	cISC4Occupant* const occupant,
	bool debugQuery,
	cIGZString& title,
	cIGZString& text)
{
	bool result = false;

	if (debugQuery && occupant)
	{
		OccupantUtil::GetDisplayName(occupant, title);

		cRZBaseString exemplarName;
		OccupantUtil::GetExemplarName(occupant, exemplarName);
		GZStringUtil::AppendLine(exemplarName, text);
		result = true;
	}

	return result;
}
