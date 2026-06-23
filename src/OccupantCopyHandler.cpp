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

#include "OccupantCopyHandler.h"
#include "ClickToCopyOccupantFilter.h"
#include "GlobalSC4InterfacePointers.h"
#include "cIGZAllocatorService.h"
#include "cIGZWin.h"
#include "cIGZWinMgr.h"
#include "cIS3DModelInstance.h"
#include "cISC43DRender.h"
#include "cISC4App.h"
#include "cISC4City.h"
#include "cISC4FloraOccupant.h"
#include "cISC4Lot.h"
#include "cISC4LotConfiguration.h"
#include "cISC4LotManager.h"
#include "cISC4Occupant.h"
#include "cISC4View3DWin.h"
#include "cISC4ViewInputControl.h"
#include "cRZAutoRefCount.h"
#include "GZServPtrs.h"
#include "SC4UI.h"

namespace
{
	class cSC4ViewInputControlPlaceLot : public cISC4ViewInputControl
	{
		uint8_t unknown[0x154];
	};
	static_assert(sizeof(cSC4ViewInputControlPlaceLot) == 0x158);

	typedef cSC4ViewInputControlPlaceLot* (__thiscall* pfn_cSC4ViewInputControlPlaceLot_ctor)(
		cSC4ViewInputControlPlaceLot* pThis,
		int32_t isBuildingPlop,
		uint32_t lotExemplarID,
		uint32_t buildingExemplarID,
		bool useMonopoly);

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
					cISC4View3DWin::ViewInputControlStackOperation_RemoveAllControls);

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
					cISC4View3DWin::ViewInputControlStackOperation_RemoveAllControls);
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

			cIS3DModelInstance* pModelInstance = nullptr;

			if (pRenderer->Pick(mouseX, mouseY, filter, pModelInstance))
			{
				cIGZUnknown* pOwner = pModelInstance->GetOwner();

				if (pOwner)
				{
					pOwner->QueryInterface(GZIID_cISC4Occupant, occupant.AsPPVoid());
				}

				pModelInstance->Release();
			}
		}

		return occupant;
	}

	cISC4LotConfiguration* GetOccupantLotConfiguration(
		cISC4View3DWin& view3D,
		int32_t mouseX,
		int32_t mouseY,
		cISC4Occupant* pOccupant)
	{
		cISC4LotConfiguration* pLotConfig = nullptr;

		if (spCity)
		{
			cISC4LotManager* pLotManager = spCity->GetLotManager();

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

						spCity->PositionToCell(data[0], data[2], cellX, cellZ);

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
}

bool OccupantCopyHandler::Execute(int32_t mouseX, int32_t mouseY)
{
	bool result = false;

	cRZAutoRefCount<cISC4View3DWin> pView3D = SC4UI::GetView3DWin();

	if (pView3D)
	{
		cRZAutoRefCount<cISC4Occupant> pOccupant = GetOccupantAtMousePosition(*pView3D, mouseX, mouseY);

		cISC4LotConfiguration* pLotConfiguration = GetOccupantLotConfiguration(
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

	return result;
}
