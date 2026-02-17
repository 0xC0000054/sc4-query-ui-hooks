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

#include "QueryUILuaExtensionsTest.h"
#include "cISC4AdvisorSystem.h"
#include "cISC4App.h"
#include "cISC4City.h"
#include "cISCLua.h"
#include "DebugUtil.h"
#include "GZServPtrs.h"

#include <array>

namespace
{
	void RunTestFunction(cIGZLua5Thread* pIGZLua5Thread, const char* functionName)
	{
		int32_t top = pIGZLua5Thread->GetTop();

		pIGZLua5Thread->GetGlobal(functionName);

		int32_t functionTop = pIGZLua5Thread->GetTop();

		if (functionTop != top)
		{
			if (pIGZLua5Thread->IsFunction(-1))
			{
				// The test functions take no parameters and return a string.

				int32_t status = pIGZLua5Thread->CallProtected(0, 1, 0, false);

				if (status == 0)
				{
					if (pIGZLua5Thread->IsString(-1))
					{
						const char* result = pIGZLua5Thread->ToString(-1);

						DebugUtil::PrintLineToDebugOutputFormatted(
							"%s() returned: %s",
							functionName,
							result ? result : "<null>");
					}
					else
					{
						DebugUtil::PrintLineToDebugOutputFormatted(
							"%s() did not return a string.",
							functionName);
					}
				}
				else
				{
					const char* errorString = pIGZLua5Thread->ToString(-1);

					DebugUtil::PrintLineToDebugOutputFormatted(
						"Error status code %d returned when calling %s(), error text: %s.",
						status,
						functionName,
						errorString ? errorString : "");

					// Pop the error string off the stack.
					pIGZLua5Thread->Pop(1);
				}
			}
			else
			{
				DebugUtil::PrintLineToDebugOutputFormatted(
					"%s is not a function, actual type: %s.",
					functionName,
					pIGZLua5Thread->TypeName(-1));
			}
		}
		else
		{
			DebugUtil::PrintLineToDebugOutputFormatted(
				"%s() does not exist.",
				functionName);
		}

		pIGZLua5Thread->SetTop(top);
	}

	void RunLuaScriptTests(cIGZLua5Thread* pIGZLua5Thread)
	{
		constexpr std::array<const char*, 3> QueryUIExtensionsLuaTestFunctions =
		{
			"null45_query_ui_extensions_test_get_exemplar_name",
			"null45_query_ui_extensions_test_get_bulldoze_cost",
			"null45_query_ui_extensions_test_get_garbage_pollution_at_center",
		};

		for (const auto& item : QueryUIExtensionsLuaTestFunctions)
		{
			RunTestFunction(pIGZLua5Thread, item);
		}
	}
}

void QueryUILuaExtensionsTest::Run()
{
#ifdef _DEBUG
	cISC4AppPtr pSC4App;

	if (pSC4App)
	{
		cISC4City* pCity = pSC4App->GetCity();

		if (pCity)
		{
			cISC4AdvisorSystem* pAdvisorSystem = pCity->GetAdvisorSystem();

			if (pAdvisorSystem)
			{
				cISCLua* pLua = pAdvisorSystem->GetScriptingContext();

				if (pLua)
				{
					RunLuaScriptTests(pLua->AsIGZLua5()->AsIGZLua5Thread());
				}
			}
		}
	}
#endif // _DEBUG
}
