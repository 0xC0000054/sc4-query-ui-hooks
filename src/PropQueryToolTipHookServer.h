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
#include "cIPropQueryToolTipHookServer.h"
#include <unordered_set>

class cISC4Occupant;
class cIGZString;

class PropQueryToolTipHookServer : public cIPropQueryToolTipHookServer
{
public:
	PropQueryToolTipHookServer();

	bool QueryInterface(uint32_t riid, void** ppvObj) override;
	uint32_t AddRef() override;
	uint32_t Release() override;

	bool HasCustomToolTipSubscribers() const;

	/**
	 * @brief Sens the custom tool tip processing message to subscribers. Stops after the first
	 * subscriber sets a custom tool tip.
	 * @param occupant The occupant that is being queried.
	 * @param title The title of the tool tip.
	 * @param text The tool tip message.
	 * @param debugQuery true if the advanced/debug query mode is active; otherwise, false.
	 * @return true if a custom tool tip was set; otherwise, false to show the game's default tool tip.
	 */
	bool SendCustomToolTipMessage(
		cISC4Occupant* const occupant,
		bool debugQuery,
		cIGZString& title,
		cIGZString& text);

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

	bool AddNotification(cIPropQueryCustomToolTipHookTarget* target) override;
	bool RemoveNotification(cIPropQueryCustomToolTipHookTarget* target) override;

	bool AddNotification(cIQueryToolTipAppendTextHookTarget* target) override;
	bool RemoveNotification(cIQueryToolTipAppendTextHookTarget* target) override;

private:
	uint32_t refCount;
	std::unordered_set<cIPropQueryCustomToolTipHookTarget*> customToolTipHookSubscribers;
	std::unordered_set<cIQueryToolTipAppendTextHookTarget*> appendToolTipHookSubscribers;
};

