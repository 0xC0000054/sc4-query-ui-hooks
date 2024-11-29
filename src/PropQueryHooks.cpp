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

#include "PropQueryHooks.h"
#include "cIGZString.h"
#include "PropQueryToolTipHookServer.h"
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

		if (spPropQueryToolTipHookServer && spPropQueryToolTipHookServer->HasCustomToolTipSubscribers())
		{
			if (spPropQueryToolTipHookServer->SendCustomToolTipMessage(pOccupant, debugQueryEnabled, title, text))
			{
				handled = true;
			}
		}

		return handled;
	}

	void SetAppendedToolTipText(cISC4Occupant* const pOccupant, bool debugQueryEnabled, cIGZString& destination)
	{
		if (spPropQueryToolTipHookServer && spPropQueryToolTipHookServer->HasAppendToolTipSubscribers())
		{
			spPropQueryToolTipHookServer->SendAppendToolTipMessage(
				pOccupant,
				debugQueryEnabled,
				destination);
		}
	}

	// Because MSVC doesn't allow free functions to use __thiscall, we have to use __fastcall
	// as a substitute. When using __fastcall the second parameter it implicitly passes in EDX
	// must be present in the function signature, but the value will never be used.

	void __fastcall HookedGetPropOccupantTipInfo(
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

void PropQueryHooks::Install()
{
	Patcher::InstallCallHook(0x4D74BF, &HookedGetPropOccupantTipInfo);
}
