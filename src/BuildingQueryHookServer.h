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

#pragma once
#include "cIBuildingQueryHookServer.h"
#include <unordered_set>

class cISC4Occupant;
class cIGZString;

class BuildingQueryHookServer final : public cIBuildingQueryHookServer
{
public:
	BuildingQueryHookServer();

	void SendBeforeQueryDialogNotifications(cISC4Occupant* pOccupant);

	void SendAfterQueryDialogNotifications(cISC4Occupant* pOccupant);

	bool HasCustomToolTipSubscribers() const;

	/**
	 * @brief Sends the custom tool tip processing message to subscribers. Stops after the first
	 * subscriber sets a custom tool tip.
	 * @param occupant The occupant that is being queried.
	 * @param debugQuery true if the advanced/debug query mode is active; otherwise, false.
	 * @param title The title of the tool tip.
	 * @param text The tool tip message.
	 * @param backgroundImageIID The instance ID of the tool tip background image, 0 for none.
	 * @param meterImageIID The instance ID of the tool tip meter (efficiency graph) image, 0 for none.
	 * @param meterPercentage The meter percentage, in the range of [0.0-1.0].
	 * @return true if a custom tool tip was set; otherwise, false to show the game's default tool tip.
	 */
	bool SendCustomToolTipMessage(
		cISC4Occupant* const occupant,
		bool debugQuery,
		cIGZString& title,
		cIGZString& text,
		uint32_t& backgroundImageIID,
		uint32_t& meterImageIID,
		float& meterPercentage);

	bool HasAppendToolTipSubscribers() const;

	/**
	 * @brief Sends the append tool tip processing message to subscribers.
	 * @param occupant The occupant that is being queried.
	 * @param debugQuery true if the advanced/debug query mode is active; otherwise, false.
	 * @param destination The string that should be appended to.
	 */
	void SendAppendToolTipMessage(
		cISC4Occupant* const occupant,
		bool debugQuery,
		cIGZString& destination);

	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;

	bool AddNotification(cIBuildingQueryDialogHookTarget* target) override;
	bool RemoveNotification(cIBuildingQueryDialogHookTarget* target) override;

	bool AddNotification(cIBuildingQueryCustomToolTipHookTarget* target) override;
	bool RemoveNotification(cIBuildingQueryCustomToolTipHookTarget* target) override;

	bool AddNotification(cIQueryToolTipAppendTextHookTarget* target) override;
	bool RemoveNotification(cIQueryToolTipAppendTextHookTarget* target) override;

private:
	uint32_t refCount;
	std::unordered_set<cIBuildingQueryDialogHookTarget*> dialogHookSubscribers;
	std::unordered_set<cIBuildingQueryCustomToolTipHookTarget*> customToolTipHookSubscribers;
	std::unordered_set<cIQueryToolTipAppendTextHookTarget*> appendToolTipHookSubscribers;
};

