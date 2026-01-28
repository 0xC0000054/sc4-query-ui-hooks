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

#include "QueryUILuaExtensions.h"
#include "cISCLua.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cISCStringDetokenizer.h"
#include "cISC4AdvisorSystem.h"
#include "cISC4Occupant.h"
#include "cRZBaseString.h"
#include "DebugUtil.h"
#include "GlobalSC4InterfacePointers.h"
#include "LuaHelper.h"
#include "Logger.h"
#include "SCLuaUtil.h"

namespace
{
	static cISC4Occupant* spOccupant = nullptr;

	int32_t get_property_value(lua_State* pState)
	{
		cRZAutoRefCount<cISCLua> lua = SCLuaUtil::GetISCLuaFromFunctionState(pState);

		if (spOccupant)
		{
			const int32_t parameterCount = lua->GetTop();

			uint32_t propertyID = 0;

			if (parameterCount == 1 && LuaHelper::GetNumber(lua, 1, propertyID))
			{
				const cISCPropertyHolder* pPropertyHolder = spOccupant->AsPropertyHolder();
				const cISCProperty* pProperty = pPropertyHolder->GetProperty(propertyID);

				if (pProperty)
				{
					LuaHelper::SetResultFromIGZVariant(lua, pProperty->GetPropertyValue());
				}
				else
				{
					lua->PushNil();
				}
			}
			else
			{
				lua->PushNil();
			}
		}
		else
		{
			lua->PushNil();
		}

		return 1;
	}

	void DebugTestLuaFunctions()
	{
#ifdef _DEBUG
		// Property id 32 is the Exemplar Name property.
		DebugUtil::PrintDetokenizedValueToDebugOutput(
			cRZBaseString("#tostring(null45_query_ui_extensions.get_property_value(32))#"));
#endif // _DEBUG
	}
}

QueryUILuaExtensions::QueryUILuaExtensions()
{
}

void QueryUILuaExtensions::PostCityInit(cISC4AdvisorSystem* pAdvisorSystem)
{
	Logger& logger = Logger::GetInstance();

	const auto status = SCLuaUtil::RegisterLuaFunction(
		pAdvisorSystem,
		"null45_query_ui_extensions",
		"get_property_value",
		&get_property_value);

	if (status == SCLuaUtil::RegisterLuaFunctionStatus::Ok)
	{
		logger.WriteLine(
			LogLevel::Info,
			"Registered the null45_query_ui_extensions.get_property_value function.");
	}
	else
	{
		logger.WriteLine(
			LogLevel::Info,
			"Failed to register the null45_query_ui_extensions.get_property_value function. "
			"Is SC4QueryUIHooks.dat in the plugins folder?");
	}
}

void QueryUILuaExtensions::PreCityShutdown()
{
	spOccupant = nullptr;
}

void QueryUILuaExtensions::BeforeDialogShown(cISC4Occupant* pOccupant)
{
	spOccupant = pOccupant;

#ifdef _DEBUG
	DebugTestLuaFunctions();
#endif // _DEBUG
}

void QueryUILuaExtensions::AfterDialogShown(cISC4Occupant* pOccupant)
{
	spOccupant = nullptr;
}