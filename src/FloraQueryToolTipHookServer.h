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

#pragma once
#include "cIFloraQueryToolTipHookServer.h"
#include <unordered_set>

class cISC4Occupant;
class cIGZString;

class FloraQueryToolTipHookServer : public cIFloraQueryToolTipHookServer
{
public:
	FloraQueryToolTipHookServer();

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

	bool AddNotification(cIFloraQueryCustomToolTipHookTarget* target) override;
	bool RemoveNotification(cIFloraQueryCustomToolTipHookTarget* target) override;

	bool AddNotification(cIQueryToolTipAppendTextHookTarget* target) override;
	bool RemoveNotification(cIQueryToolTipAppendTextHookTarget* target) override;

private:
	uint32_t refCount;
	std::unordered_set<cIFloraQueryCustomToolTipHookTarget*> customToolTipHookSubscribers;
	std::unordered_set<cIQueryToolTipAppendTextHookTarget*> appendToolTipHookSubscribers;
};

