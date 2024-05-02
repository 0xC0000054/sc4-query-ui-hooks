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

#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

class cIGZString;
class cIGZWin;

class AvailableBuildingStyles
{
public:
	static AvailableBuildingStyles& GetInstance();

	bool AppendStyleName(uint32_t style, cIGZString& destination) const;

private:
	AvailableBuildingStyles();

	static bool BuildingStyleWinEnumProc(
		cIGZWin* parent,
		uint32_t childID,
		cIGZWin* child,
		void* pState);

	std::unordered_map<uint32_t, std::string> availableStyles;
};

