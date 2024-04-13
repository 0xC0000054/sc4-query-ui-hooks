///////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-query-ui-hooks, a DLL Plugin for SimCity 4 that
// provides more data for the query UI.
//
// Copyright (c) 2024 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
///////////////////////////////////////////////////////////////////////////////

#include "BuildingQueryHooks.h"
#include "BuildingQueryHookServer.h"
#include "DebugUtil.h"
#include "GlobalHookServerPointers.h"
#include "Logger.h"
#include "Patcher.h"
#include "QueryToolHelpers.h"
#include "cGZPersistResourceKey.h"
#include "cIGZString.h"
#include "cIGZVariant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cISC4Occupant.h"
#include "cRZAutoRefCount.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"

namespace
{
	typedef bool(__cdecl* pfnDoQueryDialog)(cISC4Occupant* occupant);

	static const pfnDoQueryDialog RealDoQueryDialog = reinterpret_cast<pfnDoQueryDialog>(0x4D3C30);

	bool __cdecl HookedDoQueryDialog(cISC4Occupant* pOccupant)
	{
#ifdef _DEBUG
		DebugUtil::PrintOccupantNameToDebugOutput(pOccupant);
#endif // _DEBUG

		if (spBuildingQueryHookServer)
		{
			spBuildingQueryHookServer->SendBeforeQueryDialogNotifications(pOccupant);
		}

		bool result = RealDoQueryDialog(pOccupant);

		if (spBuildingQueryHookServer)
		{
			spBuildingQueryHookServer->SendAfterQueryDialogNotifications(pOccupant);
		}

		return result;
	}

	typedef void(__thiscall* pfnGetBuildingOccupantTipInfo)(
		void* pThis,
		cIGZString& title,
		cIGZString& text,
		uint32_t& tipBackgroundImage,
		uint32_t& tipMeterImage,
		float& meterPercentage);

	static const pfnGetBuildingOccupantTipInfo RealGetBuildingOccupantTipInfo = reinterpret_cast<pfnGetBuildingOccupantTipInfo>(0x4D5E30);

	bool SetCustomToolTip(
		void* thisPtr,
		cIGZString& title,
		cIGZString& text,
		uint32_t& backgroundImageIID,
		uint32_t& meterImageIID,
		float& meterPercentage)
	{
		bool result = false;

		if (spBuildingQueryHookServer && spBuildingQueryHookServer->HasCustomToolTipSubscribers())
		{
			const bool debugQueryEnabled = QueryToolHelpers::IsDebugQueryEnabled();
			cISC4Occupant* const pOccupant = QueryToolHelpers::GetOccupant(thisPtr);

			result = spBuildingQueryHookServer->SendCustomToolTipMessage(
				pOccupant,
				debugQueryEnabled,
				title,
				text,
				backgroundImageIID,
				meterImageIID,
				meterPercentage);
		}

		return result;
	}

	void SetAppendedToolTipText(void* thisPtr, cIGZString& destination)
	{
		if (spBuildingQueryHookServer && spBuildingQueryHookServer->HasAppendToolTipSubscribers())
		{
			const bool debugQueryEnabled = QueryToolHelpers::IsDebugQueryEnabled();
			cISC4Occupant* const pOccupant = QueryToolHelpers::GetOccupant(thisPtr);

			spBuildingQueryHookServer->SendAppendToolTipMessage(
				pOccupant,
				debugQueryEnabled,
				destination);
		}
	}

	// Because MSVC doesn't allow free functions to use __thiscall, we have to use __fastcall
	// as a substitute. When using __fastcall the second parameter it implicitly passes in EDX
	// must be present in the function signature, but the value will never be used.

	void __fastcall HookedGetBuildingOccupantTipInfo(
		void* thisPtr,
		void* edxUnused,
		cIGZString& title,
		cIGZString& text,
		uint32_t& backgroundImageIID,
		uint32_t& meterImageIID,
		float& meterPercentage)
	{
		if (!SetCustomToolTip(thisPtr, title, text, backgroundImageIID, meterImageIID, meterPercentage))
		{
			RealGetBuildingOccupantTipInfo(thisPtr, title, text, backgroundImageIID, meterImageIID, meterPercentage);
			SetAppendedToolTipText(thisPtr, text);
		}
	}

	void InstallDoQueryDialogHook()
	{
		constexpr uintptr_t cSC4ViewInputControlQuery_DescribePick_Inject = 0x4D4494;
		constexpr uintptr_t LUAExtension_window_query_Inject = 0x40CA7A;

		Patcher::InstallCallHook(cSC4ViewInputControlQuery_DescribePick_Inject, &HookedDoQueryDialog);
		Patcher::InstallCallHook(LUAExtension_window_query_Inject, &HookedDoQueryDialog);
	}

	void InstallGetBuildingOccupantTipInfoHook()
	{
		constexpr uintptr_t cSC4ViewInputControlQuery_DoMessage_Inject = 0x4D7464;

		Patcher::InstallCallHook(cSC4ViewInputControlQuery_DoMessage_Inject, &HookedGetBuildingOccupantTipInfo);
	}
}

void BuildingQueryHooks::Install()
{
	InstallDoQueryDialogHook();
	InstallGetBuildingOccupantTipInfoHook();
}
