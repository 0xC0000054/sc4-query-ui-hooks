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
#include "cIGZAllocatorService.h"
#include "cIGZString.h"
#include "cIGZVariant.h"
#include "cIGZWin.h"
#include "cIGZWinMgr.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cISC4App.h"
#include "cISC4City.h"
#include "cISC4Lot.h"
#include "cISC4LotConfiguration.h"
#include "cISC4LotManager.h"
#include "cISC4Occupant.h"
#include "cISC4View3DWin.h"
#include "cISC4ViewInputControl.h"
#include "cRZAutoRefCount.h"
#include "GZServPtrs.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"

#include <Windows.h>

namespace
{
	bool IsKeyDownNow(int virtualKeyCode)
	{
		if (virtualKeyCode == VK_LBUTTON)
		{
			if (GetSystemMetrics(SM_SWAPBUTTON))
			{
				// The left and right mouse buttons are swapped.
				virtualKeyCode = VK_RBUTTON;
			}
		}

		return (GetAsyncKeyState(virtualKeyCode) & 0x8000) != 0;
	}

	uint32_t GetOccupantLotExemplarID(cISC4App& sc4App,	cISC4Occupant* pOccupant)
	{
		uint32_t lotExemplarId = 0;

		cISC4City* pCity = sc4App.GetCity();

		if (pCity)
		{
			cISC4LotManager* pLotManager = pCity->GetLotManager();

			if (pLotManager)
			{
				cISC4Lot* pOccupantLot = pLotManager->GetOccupantLot(pOccupant);

				if (pOccupantLot)
				{
					cISC4LotConfiguration* pLotConfiguration = pOccupantLot->GetLotConfiguration();

					if (pLotConfiguration)
					{
						lotExemplarId = pLotConfiguration->GetID();
					}
				}
			}
		}

		return lotExemplarId;
	}

	class cSC4ViewInputControlPlaceLot : public cISC4ViewInputControl
	{
		uint8_t unknown[0x154];
	};
	static_assert(sizeof(cSC4ViewInputControlPlaceLot) == 0x158);

	typedef cSC4ViewInputControlPlaceLot* (__thiscall* pfn_cSC4ViewInputControlPlaceLot_ctor)(
		cSC4ViewInputControlPlaceLot* pThis,
		int32_t isBuildingPlop,
		uint32_t lotExemplarID,
		uint32_t unknown1,
		bool unknown2);

	static const pfn_cSC4ViewInputControlPlaceLot_ctor cSC4ViewInputControlPlaceLot_ctor = reinterpret_cast<pfn_cSC4ViewInputControlPlaceLot_ctor>(0x4c0430);

	bool CreateViewInputControlPlaceLot(uint32_t lotExemplarID, cRZAutoRefCount<cSC4ViewInputControlPlaceLot>& instance)
	{
		bool result = false;

		cIGZAllocatorServicePtr pAllocatorService;

		if (pAllocatorService)
		{
			auto pControl = static_cast<cSC4ViewInputControlPlaceLot*>(pAllocatorService->Allocate(sizeof(cSC4ViewInputControlPlaceLot)));

			if (pControl)
			{
				cSC4ViewInputControlPlaceLot_ctor(pControl, 0, lotExemplarID, 0, false);

				instance = pControl;
				result = true;
			}
		}

		return result;
	}

	typedef cIGZWin* (__cdecl* pfnGetChildWindowForCursorPosRecursive)(cIGZWin* pWin, int32_t cursorX, int32_t cursorZ);

	static const pfnGetChildWindowForCursorPosRecursive GetChildWindowForCursorPosRecursive = reinterpret_cast<pfnGetChildWindowForCursorPosRecursive>(0x78d720);

	bool PickOccupantLot(cISC4Occupant* pOccupant)
	{
		constexpr uint32_t kGZWin_WinSC4App = 0x6104489A;
		constexpr uint32_t kGZWin_SC4View3DWin = 0x9a47b417;
		constexpr uint32_t kGZIID_cISC4View3DWin = 0xFA47B3F9;

		bool result = false;

		cISC4AppPtr pSC4App;
		cIGZWinMgrPtr pWM;

		if (pSC4App && pWM)
		{
			const uint32_t lotExemplarID = GetOccupantLotExemplarID(*pSC4App, pOccupant);

			if (lotExemplarID != 0)
			{
				cIGZWin* pMainWindow = pSC4App->GetMainWindow();

				if (pMainWindow)
				{
					cIGZWin* pWinSC4App = pMainWindow->GetChildWindowFromID(kGZWin_WinSC4App);

					if (pWinSC4App)
					{
						cRZAutoRefCount<cISC4View3DWin> pView3D;

						if (pWinSC4App->GetChildAs(
							kGZWin_SC4View3DWin,
							kGZIID_cISC4View3DWin,
							pView3D.AsPPVoid()))
						{
							cRZAutoRefCount<cSC4ViewInputControlPlaceLot> pPlaceLot;

							if (CreateViewInputControlPlaceLot(lotExemplarID, pPlaceLot))
							{
								pView3D->SetCurrentViewInputControl(
									pPlaceLot,
									cISC4View3DWin::ViewInputControlStackOperation::RemoveAllControls);

								int32_t cursorX = 0;
								int32_t cursorZ = 0;

								pWM->GetCursorScreenPosition(cursorX, cursorZ);

								cIGZWin* pView3DAsIGZWin = pView3D->AsIGZWin();

								cIGZWin* pCursorWin = GetChildWindowForCursorPosRecursive(
									pView3DAsIGZWin->GetParentWin(),
									cursorX,
									cursorZ);

								if (pCursorWin == pView3DAsIGZWin)
								{
									pPlaceLot->OnMouseMove(cursorX, cursorZ, 0);
								}

								result = true;
							}
						}
					}
				}
			}
		}

		return result;
	}

	bool OccupantIsBuilding(cISC4Occupant* pOccupant)
	{
		constexpr uint32_t kOccupantType_Building = 0x278128A0;

		return pOccupant && pOccupant->GetType() == kOccupantType_Building;
	}

	typedef bool(__cdecl* pfnDoQueryDialog)(cISC4Occupant* occupant);

	static const pfnDoQueryDialog RealDoQueryDialog = reinterpret_cast<pfnDoQueryDialog>(0x4D3C30);

	bool __cdecl HookedDoQueryDialog(cISC4Occupant* pOccupant)
	{
#ifdef _DEBUG
		DebugUtil::PrintOccupantNameToDebugOutput(pOccupant);
#endif // _DEBUG

		bool result = false;

		if (IsKeyDownNow(VK_SHIFT)
			&& !IsKeyDownNow(VK_CONTROL)
			&& !IsKeyDownNow(VK_MENU)
			&& OccupantIsBuilding(pOccupant))
		{
			// If shift is the only modifier key being pressed, clicking on a building
			// will allow the user to plop a duplicate lot.

			result = PickOccupantLot(pOccupant);
		}
		else
		{
			if (spBuildingQueryHookServer)
			{
				spBuildingQueryHookServer->SendBeforeQueryDialogNotifications(pOccupant);
			}

			result = RealDoQueryDialog(pOccupant);

			if (spBuildingQueryHookServer)
			{
				spBuildingQueryHookServer->SendAfterQueryDialogNotifications(pOccupant);
			}
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

	static void __cdecl HookedPlayOccupantQuerySound(cISC4Occupant* pOccupant)
	{
		// Do nothing.
	}

	void InstallPlayOccupantQuerySoundHook()
	{
		constexpr uintptr_t cSC4ViewInputControlQuery_DescribePick_Inject = 0x4D4406;

		Patcher::InstallCallHook(cSC4ViewInputControlQuery_DescribePick_Inject, &HookedPlayOccupantQuerySound);
	}
}

void BuildingQueryHooks::Install(const Settings& settings)
{
	InstallDoQueryDialogHook();
	InstallGetBuildingOccupantTipInfoHook();

	if (!settings.EnableOccupantQuerySounds())
	{
		InstallPlayOccupantQuerySoundHook();
	}
}
