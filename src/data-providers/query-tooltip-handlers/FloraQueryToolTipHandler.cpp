/*
 * This file is part of sc4-query-ui-hooks, a DLL Plugin for SimCity 4 that
 * extends the query UI.
 *
 * Copyright (C) 2024, 2025, 2026 Nicholas Hayes
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

#include "FloraQueryToolTipHandler.h"
#include "cIFloraQueryToolTipHookServer.h"
#include "cIGZLanguageUtility.h"
#include "cISC4City.h"
#include "cISC4FloraOccupant.h"
#include "cISC4Occupant.h"
#include "cISC4Simulator.h"
#include "GZStringUtil.h"
#include "OccupantUtil.h"
#include "GZServPtrs.h"

FloraQueryToolTipHandler::FloraQueryToolTipHandler()
	: refCount(0), pDate(nullptr), pLM(nullptr)
{
}

void FloraQueryToolTipHandler::PostCityInit(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM)
{
	cISC4City* pCity = static_cast<cISC4City*>(pStandardMsg->GetVoid1());
	pCity->GetSimulator()->GetSimDate()->Clone(&pDate);

	cRZAutoRefCount<cIFloraQueryToolTipHookServer> hookServer;

	if (pCOM->GetClassObject(
		GZCLSID_cIFloraQueryToolTipHookServer,
		GZIID_cIFloraQueryToolTipHookServer,
		hookServer.AsPPVoid()))
	{
		hookServer->AddNotification(this);
	}
}

void FloraQueryToolTipHandler::PreCityShutdown(cIGZMessage2Standard* pStandardMsg, cIGZCOM* pCOM)
{
	cIGZDate* localDate = pDate;
	pDate = nullptr;

	if (localDate)
	{
		localDate->Release();
	}

	cRZAutoRefCount<cIFloraQueryToolTipHookServer> hookServer;

	if (pCOM->GetClassObject(
		GZCLSID_cIFloraQueryToolTipHookServer,
		GZIID_cIFloraQueryToolTipHookServer,
		hookServer.AsPPVoid()))
	{
		hookServer->RemoveNotification(this);
	}
}

void FloraQueryToolTipHandler::PostAppInit(cIGZCOM* pCOM)
{
	cIGZLanguageManagerPtr languageManager;
	pLM = languageManager;
	languageManager->AddRef();
}

void FloraQueryToolTipHandler::PreAppShutdown(cIGZCOM* pCOM)
{
	cIGZLanguageManager* localLM = pLM;
	pLM = nullptr;

	if (localLM)
	{
		localLM->Release();
	}
}

bool FloraQueryToolTipHandler::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIFloraQueryCustomToolTipHookTarget)
	{
		*ppvObj = static_cast<cIFloraQueryCustomToolTipHookTarget*>(this);
		AddRef();

		return true;
	}

	return QueryToolTipHandlerBase::QueryInterface(riid, ppvObj);
}

uint32_t FloraQueryToolTipHandler::AddRef()
{
	return QueryToolTipHandlerBase::AddRef();
}

uint32_t FloraQueryToolTipHandler::Release()
{
	return QueryToolTipHandlerBase::Release();
}

bool FloraQueryToolTipHandler::ProcessToolTip(
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

		cRZAutoRefCount<cISC4FloraOccupant> floraOccupant;

		if (occupant->QueryInterface(GZIID_cISC4FloraOccupant, floraOccupant.AsPPVoid()))
		{
			GZStringUtil::AppendLineFormatted(
				text,
				"Birth date: %s",
				GetDateNumberString(floraOccupant->GetBirthDate()).ToChar());
			GZStringUtil::AppendLineFormatted(
				text,
				"Last seeding date: %s",
				GetDateNumberString(floraOccupant->GetLastSeedingDate()).ToChar());
		}
		result = true;
	}

	return result;
}

cRZBaseString FloraQueryToolTipHandler::GetDateNumberString(uint32_t dateNumber)
{
	cRZBaseString result;

	pDate->Set(dateNumber);

	uint32_t month = pDate->Month();
	uint32_t day = pDate->DayOfMonth();
	uint32_t year = pDate->Year();

	pLM->GetLanguageUtility(0)->MakeDateString(month, day, year, result, 0);

	return result;
}
