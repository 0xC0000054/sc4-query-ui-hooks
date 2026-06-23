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

#include "BuildingQueryHooks.h"
#include "BuildingQueryHookServer.h"
#include "DebugUtil.h"
#include "GlobalHookServerPointers.h"
#include "Logger.h"
#include "OccupantCopyHandler.h"
#include "OccupantUtil.h"
#include "Patcher.h"
#include "QueryToolHelpers.h"
#include "cIGZString.h"
#include "cISC4Occupant.h"

#include <Windows.h>

namespace
{
	typedef bool(__cdecl* pfnDoQueryDialog)(cISC4Occupant* occupant);

	static const pfnDoQueryDialog RealDoQueryDialog = reinterpret_cast<pfnDoQueryDialog>(0x4D3C30);

	bool __cdecl HookedDoQueryDialog(cISC4Occupant* pOccupant)
	{
#ifdef _DEBUG
		cRZAutoRefCount<cIGZString> name;

		if (OccupantUtil::GetUserVisibleName(pOccupant, name))
		{
			DebugUtil::PrintLineToDebugOutputUtf8(name->ToChar());
		}
#endif // _DEBUG

		bool result = false;

		if (spBuildingQueryHookServer)
		{
			spBuildingQueryHookServer->SendBeforeQueryDialogNotifications(pOccupant);
		}

		result = RealDoQueryDialog(pOccupant);

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

	void __cdecl HookedPlayOccupantQuerySound(cISC4Occupant* pOccupant)
	{
		// Do nothing.
	}

	void InstallPlayOccupantQuerySoundHook()
	{
		constexpr uintptr_t cSC4ViewInputControlQuery_DescribePick_Inject = 0x4D4406;

		Patcher::InstallCallHook(cSC4ViewInputControlQuery_DescribePick_Inject, &HookedPlayOccupantQuerySound);
	}

	enum ModifierKeysFlags : uint32_t
	{
		ModifierKeyFlagShift = 0x1,
		ModifierKeyFlagControl = 0x2,
		ModifierKeyFlagAlt = 0x4,
		ModifierKeyFlagAll = ModifierKeyFlagShift | ModifierKeyFlagControl | ModifierKeyFlagAlt,
	};

	bool ProcessQueryToolClick(int32_t mouseX, int32_t mouseY, int32_t modifierKeys)
	{
		bool result = false;

		const int32_t activeModifierKeys = modifierKeys & ModifierKeyFlagAll;

		if (activeModifierKeys == ModifierKeyFlagShift)
		{
			result = OccupantCopyHandler::Execute(mouseX, mouseY);
		}

		return result;
	}

	static constexpr uintptr_t StandardQueryBehavior_Continue = 0x4D4F19;
	static constexpr uintptr_t ProcessQueryToolClick_Continue = 0x4D4F30;

	void NAKED_FUN HookedOnMouseDownL()
	{
		__asm
		{
			// EAX, ECX and EDX do not need to be preserved.
			cmp dword ptr[esi + 0x8c], 0x1
			// Continue with the standard query tool behavior if the
			// traffic query tool is active.
			jz standardQueryContinue
			mov al, byte ptr[esp + 0x2c]
			and al, ModifierKeyFlagAll
			test al, al
			// Continue with the standard query tool behavior if the
			// no modifier keys are pressed.
			jz standardQueryContinue
			mov eax, dword ptr[esp + 0x2c]
			mov ecx, dword ptr[esp + 0x28]
			mov edx, dword ptr[esp + 0x24]
			push eax // modifier keys
			push ecx // y
			push edx // x
			call ProcessQueryToolClick // (cdecl)
			add esp, 12
			test al,al
			jz standardQueryContinue
			jmp ProcessQueryToolClick_Continue

			standardQueryContinue:
			jmp StandardQueryBehavior_Continue
		}
	}

	void InstallOnMouseDownLHook()
	{
		constexpr uintptr_t cSC4ViewInputControlQuery_OnMouseDownL_Inject = 0x4D4EAA;

		Patcher::InstallJump(cSC4ViewInputControlQuery_OnMouseDownL_Inject, reinterpret_cast<uintptr_t>(&HookedOnMouseDownL));
	}
}

void BuildingQueryHooks::Install(const ISettings& settings)
{
	InstallDoQueryDialogHook();
	InstallGetBuildingOccupantTipInfoHook();
	InstallOnMouseDownLHook();

	if (!settings.EnableOccupantQuerySounds())
	{
		InstallPlayOccupantQuerySoundHook();
	}
}
