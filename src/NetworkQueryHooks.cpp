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

#include "NetworkQueryHooks.h"
#include "NetworkQueryToolTipHookServer.h"
#include "GlobalHookServerPointers.h"
#include "DebugUtil.h"
#include "Patcher.h"
#include "QueryToolHelpers.h"
#include "cIGZString.h"
#include <Windows.h>

namespace
{
	typedef void(__thiscall* pfnGetNetworkOccupantTipInfo)(
		void* thisPtr,
		cIGZString& title,
		cIGZString& text);

	static const pfnGetNetworkOccupantTipInfo RealGetNetworkOccupantTipInfo = reinterpret_cast<pfnGetNetworkOccupantTipInfo>(0x4CF8A0);

	bool SetCustomToolTip(
		void* thisPtr,
		cIGZString& title,
		cIGZString& text)
	{
		bool handled = false;

		if (spNetworkQueryToolTipHookServer && spNetworkQueryToolTipHookServer->HasCustomToolTipSubscribers())
		{
			const bool debugQueryEnabled = QueryToolHelpers::IsDebugQueryEnabled();
			cISC4Occupant* const pOccupant = QueryToolHelpers::GetOccupant(thisPtr);

			if (spNetworkQueryToolTipHookServer->SendCustomToolTipMessage(pOccupant, debugQueryEnabled, title, text))
			{
				handled = true;
			}
		}

		return handled;
	}

	void SetAppendedToolTipText(void* thisPtr, cIGZString& destination)
	{
		if (spNetworkQueryToolTipHookServer && spNetworkQueryToolTipHookServer->HasAppendToolTipSubscribers())
		{
			const bool debugQueryEnabled = QueryToolHelpers::IsDebugQueryEnabled();
			cISC4Occupant* const pOccupant = QueryToolHelpers::GetOccupant(thisPtr);

			spNetworkQueryToolTipHookServer->SendAppendToolTipMessage(
				pOccupant,
				debugQueryEnabled,
				destination);
		}
	}

	// Because MSVC doesn't allow free functions to use __thiscall, we have to use __fastcall
	// as a substitute. When using __fastcall the second parameter it implicitly passes in EDX
	// must be present in the function signature, but the value will never be used.

	void __fastcall HookedGetNetworkOccupantTipInfo(
		void* thisPtr,
		void* edxUnused,
		cIGZString& title,
		cIGZString& text)
	{
		if (!SetCustomToolTip(thisPtr, title, text))
		{
			RealGetNetworkOccupantTipInfo(thisPtr, title, text);
			SetAppendedToolTipText(thisPtr, text);
		}
	}
}

void NetworkQueryHooks::Install()
{
	Patcher::InstallCallHook(0x4D74A8, &HookedGetNetworkOccupantTipInfo);
}
