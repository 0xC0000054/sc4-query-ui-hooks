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

#include "BuildingQueryHooks.h"
#include "BuildingQueryHookServer.h"
#include "ClickToCopyOccupantFilter.h"
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
#include "cIS3DModelInstance.h"
#include "cISC43DRender.h"
#include "cISC4App.h"
#include "cISC4City.h"
#include "cISC4FloraOccupant.h"
#include "cISC4Lot.h"
#include "cISC4LotConfiguration.h"
#include "cISC4LotManager.h"
#include "cISC4Occupant.h"
#include "cISC4OccupantFilter.h"
#include "cISC4View3DWin.h"
#include "cISC4ViewInputControl.h"
#include "cRZAutoRefCount.h"
#include "GZServPtrs.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"

#include <Windows.h>

namespace
{
	typedef bool(__cdecl* pfnDoQueryDialog)(cISC4Occupant* occupant);

	static const pfnDoQueryDialog RealDoQueryDialog = reinterpret_cast<pfnDoQueryDialog>(0x4D3C30);

	bool __cdecl HookedDoQueryDialog(cISC4Occupant* pOccupant)
	{
#ifdef _DEBUG
		DebugUtil::PrintOccupantNameToDebugOutput(pOccupant);
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

	class cSC4ViewInputControlFlora : public cISC4ViewInputControl
	{
		uint8_t unknown[0x154];
	};
	static_assert(sizeof(cSC4ViewInputControlFlora) == 0x158);

	typedef cSC4ViewInputControlFlora* (__thiscall* pfn_cSC4ViewInputControlFlora_ctor)(cSC4ViewInputControlFlora* pThis, uint32_t floraType);

	static const pfn_cSC4ViewInputControlFlora_ctor cSC4ViewInputControlFlora_ctor = reinterpret_cast<pfn_cSC4ViewInputControlFlora_ctor>(0x4bb900);

	bool CreateViewInputControlFlora(uint32_t floraType, cRZAutoRefCount<cSC4ViewInputControlFlora>& instance)
	{
		bool result = false;

		cIGZAllocatorServicePtr pAllocatorService;

		if (pAllocatorService)
		{
			auto pControl = static_cast<cSC4ViewInputControlFlora*>(pAllocatorService->Allocate(sizeof(cSC4ViewInputControlFlora)));

			if (pControl)
			{
				cSC4ViewInputControlFlora_ctor(pControl, floraType);

				instance = pControl;
				result = true;
			}
		}

		return result;
	}

	bool CopyLot(cISC4View3DWin& view3D, uint32_t lotExemplarID)
	{
		bool result = false;

		cIGZWinMgrPtr pWM;

		if (lotExemplarID != 0 && pWM)
		{
			cRZAutoRefCount<cSC4ViewInputControlPlaceLot> placeLot;

			if (CreateViewInputControlPlaceLot(lotExemplarID, placeLot))
			{
				view3D.SetCurrentViewInputControl(
					placeLot,
					cISC4View3DWin::ViewInputControlStackOperation::RemoveAllControls);

				int32_t cursorX = 0;
				int32_t cursorZ = 0;

				pWM->GetCursorScreenPosition(cursorX, cursorZ);

				cIGZWin* pView3DAsIGZWin = view3D.AsIGZWin();

				cIGZWin* pCursorWin = GetChildWindowForCursorPosRecursive(
					pView3DAsIGZWin->GetParentWin(),
					cursorX,
					cursorZ);

				if (pCursorWin == pView3DAsIGZWin)
				{
					placeLot->OnMouseMove(cursorX, cursorZ, 0);
				}

				result = true;
			}
		}

		return result;
	}

	bool CopyFloraOccupant(cISC4View3DWin& view3D, cISC4FloraOccupant& floraOccupant)
	{
		bool result = false;

		const uint32_t type = floraOccupant.GetFloraType();

		constexpr uint32_t kDeadFloraOccupant = 0x29CFF3CB;

		if (type != 0 && type != kDeadFloraOccupant)
		{
			cRZAutoRefCount<cSC4ViewInputControlFlora> flora;

			if (CreateViewInputControlFlora(type, flora))
			{
				result = view3D.SetCurrentViewInputControl(
					flora,
					cISC4View3DWin::ViewInputControlStackOperation::RemoveAllControls);
			}
		}

		return result;
	}

	cRZAutoRefCount<cISC4Occupant> GetOccupantAtMousePosition(
		cISC4View3DWin& view3D,
		int32_t mouseX,
		int32_t mouseY)
	{
		cRZAutoRefCount<cISC4Occupant> occupant;

		cISC43DRender* pRenderer = view3D.GetRenderer();

		if (pRenderer)
		{
			cRZAutoRefCount<cISC4OccupantFilter> filter(
				new ClickToCopyOccupantFilter(),
				cRZAutoRefCount<cISC4OccupantFilter>::kAddRef);

			cRZAutoRefCount<cIS3DModelInstance> modelInstance;

			if (pRenderer->Pick(mouseX, mouseY, filter, modelInstance.AsPPObj()))
			{
				cIGZUnknown* pOwner = modelInstance->GetOwner();

				if (pOwner)
				{
					pOwner->QueryInterface(GZIID_cISC4Occupant, occupant.AsPPVoid());
				}
			}
		}

		return occupant;
	}

	cISC4LotConfiguration* GetOccupantLotConfiguration(
		cISC4App& sc4App,
		cISC4View3DWin& view3D,
		int32_t mouseX,
		int32_t mouseY,
		cISC4Occupant* pOccupant)
	{
		cISC4LotConfiguration* pLotConfig = nullptr;

		cISC4City* pCity = sc4App.GetCity();

		if (pCity)
		{
			cISC4LotManager* pLotManager = pCity->GetLotManager();

			if (pLotManager)
			{
				cISC4Lot* pLot = nullptr;

				if (pOccupant)
				{
					pLot = pLotManager->GetOccupantLot(pOccupant);
				}
				else
				{
					float data[3]{};

					if (view3D.PickTerrain(mouseX, mouseY, data, false))
					{
						int32_t cellX = 0;
						int32_t cellZ = 0;

						pCity->PositionToCell(data[0], data[2], cellX, cellX);

						pLot = pLotManager->GetLot(cellX, cellZ, false);
					}
				}

				if (pLot)
				{
					pLotConfig = pLot->GetLotConfiguration();
				}
			}
		}

		return pLotConfig;
	}

	cRZAutoRefCount<cISC4View3DWin> GetView3D(cISC4App& sc4App)
	{
		constexpr uint32_t kGZWin_WinSC4App = 0x6104489A;
		constexpr uint32_t kGZWin_SC4View3DWin = 0x9a47b417;
		constexpr uint32_t kGZIID_cISC4View3DWin = 0xFA47B3F9;

		cRZAutoRefCount<cISC4View3DWin> view3D;

		cIGZWin* pMainWindow = sc4App.GetMainWindow();

		if (pMainWindow)
		{
			cIGZWin* pWinSC4App = pMainWindow->GetChildWindowFromID(kGZWin_WinSC4App);

			if (pWinSC4App)
			{
				pWinSC4App->GetChildAs(
					kGZWin_SC4View3DWin,
					kGZIID_cISC4View3DWin,
					view3D.AsPPVoid());
			}
		}

		return view3D;
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
			cISC4AppPtr pSC4App;

			if (pSC4App)
			{
				cRZAutoRefCount<cISC4View3DWin> pView3D = GetView3D(*pSC4App);

				if (pView3D)
				{
					cRZAutoRefCount<cISC4Occupant> pOccupant = GetOccupantAtMousePosition(*pView3D, mouseX, mouseY);

					cISC4LotConfiguration* pLotConfiguration = GetOccupantLotConfiguration(
						*pSC4App,
						*pView3D,
						mouseX,
						mouseY,
						pOccupant);

					if (pLotConfiguration)
					{
						result = CopyLot(*pView3D, pLotConfiguration->GetID());
					}
					else if (pOccupant)
					{
						cRZAutoRefCount<cISC4FloraOccupant> pFloraOccupant;

						if (pOccupant->QueryInterface(GZIID_cISC4FloraOccupant, pFloraOccupant.AsPPVoid()))
						{
							result = CopyFloraOccupant(*pView3D, *pFloraOccupant);
						}
					}
				}
			}
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
