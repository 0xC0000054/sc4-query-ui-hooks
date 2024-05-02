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

#include "AvailableBuildingStyles.h"
#include "cIGZString.h"
#include "cIGZWin.h"
#include "cIGZWinBtn.h"
#include "cISC4App.h"
#include "cRZAutoRefCount.h"
#include "GZServPtrs.h"

AvailableBuildingStyles& AvailableBuildingStyles::GetInstance()
{
	static AvailableBuildingStyles instance;

	return instance;
}

bool AvailableBuildingStyles::AppendStyleName(uint32_t style, cIGZString& destination) const
{
	bool result = false;

	const auto& item = availableStyles.find(style);

	if (item != availableStyles.end())
	{
		const std::string& name = item->second;

		destination.Append(name.c_str(), name.size());
		result = true;
	}

	return result;
}

AvailableBuildingStyles::AvailableBuildingStyles()
{
	constexpr uint32_t kGZWin_WinSC4App = 0x6104489A;
	constexpr uint32_t BuildingStyleListContainer = 0x8BCA20C3;
	constexpr uint32_t GZIID_cIGZWinBtn = 0x8810;

	cISC4AppPtr pSC4App;

	if (pSC4App)
	{
		cIGZWin* mainWindow = pSC4App->GetMainWindow();

		if (mainWindow)
		{
			cIGZWin* pSC4AppWin = mainWindow->GetChildWindowFromID(kGZWin_WinSC4App);

			if (pSC4AppWin)
			{
				// Get the child window that contains the building style radio buttons.

				cIGZWin* pStyleListContainer = pSC4AppWin->GetChildWindowFromIDRecursive(BuildingStyleListContainer);

				if (pStyleListContainer)
				{
					// Enumerate the buttons in the child window to get the list of available styles.
					pStyleListContainer->EnumChildren(
						GZIID_cIGZWinBtn,
						&BuildingStyleWinEnumProc,
						this);
				}
			}
		}
	}
}

bool AvailableBuildingStyles::BuildingStyleWinEnumProc(cIGZWin* parent, uint32_t childID, cIGZWin* child, void* pState)
{
	constexpr uint32_t titleBarButton = 0x2BC619F3;
	constexpr uint32_t minimizeButton = 0xEBC619FD;

	// The title bar and minimize buttons are excluded, every other button
	// in the dialog is a style radio button.

	if (childID != titleBarButton && childID != minimizeButton)
	{
		cRZAutoRefCount<cIGZWinBtn> pBtn;

		if (child->QueryInterface(GZIID_cIGZWinBtn, pBtn.AsPPVoid()))
		{
			cIGZString* caption = pBtn->GetCaption();

			AvailableBuildingStyles* context = static_cast<AvailableBuildingStyles*>(pState);

			context->availableStyles.try_emplace(childID, caption->ToChar());
		}
	}

	return true;
}
