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

#include "FloraQueryHooks.h"
#include "cIGZString.h"
#include "FloraQueryToolTipHookServer.h"
#include "GlobalHookServerPointers.h"
#include "OccupantUtil.h"
#include "Patcher.h"
#include "QueryToolHelpers.h"

namespace
{

	bool SetCustomToolTip(
		cISC4Occupant* const pOccupant,
		bool debugQueryEnabled,
		cIGZString& title,
		cIGZString& text)
	{
		bool handled = false;

		if (spFloraQueryToolTipHookServer && spFloraQueryToolTipHookServer->HasCustomToolTipSubscribers())
		{
			if (spFloraQueryToolTipHookServer->SendCustomToolTipMessage(pOccupant, debugQueryEnabled, title, text))
			{
				handled = true;
			}
		}

		return handled;
	}

	void SetAppendedToolTipText(cISC4Occupant* const pOccupant, bool debugQueryEnabled, cIGZString& destination)
	{
		if (spFloraQueryToolTipHookServer && spFloraQueryToolTipHookServer->HasAppendToolTipSubscribers())
		{
			spFloraQueryToolTipHookServer->SendAppendToolTipMessage(
				pOccupant,
				debugQueryEnabled,
				destination);
		}
	}

	// Because MSVC doesn't allow free functions to use __thiscall, we have to use __fastcall
	// as a substitute. When using __fastcall the second parameter it implicitly passes in EDX
	// must be present in the function signature, but the value will never be used.

	void __fastcall HookedGetFloraOccupantTipInfo(
		void* thisPtr,
		void* edxUnused,
		cIGZString& title,
		cIGZString& text)
	{
		const bool debugQuery = QueryToolHelpers::IsDebugQueryEnabled();
		cISC4Occupant* const pOccupant = QueryToolHelpers::GetOccupant(thisPtr);

		if (!SetCustomToolTip(pOccupant, debugQuery, title, text))
		{
			OccupantUtil::GetDisplayName(pOccupant, title);
			SetAppendedToolTipText(pOccupant, debugQuery, text);
		}
	}
}

void FloraQueryHooks::Install()
{
	Patcher::InstallCallHook(0x4D747F, &HookedGetFloraOccupantTipInfo);
}
