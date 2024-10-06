///////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-query-ui-hooks, a DLL Plugin for SimCity 4 that
// extends the query UI.
//
// Copyright (c) 2024 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <cstdint>
#include <string_view>

class cIGZString;

namespace GZStringUtil
{
	void AppendLine(const std::string_view& line, cIGZString& destination);
	void AppendLine(const cIGZString& line, cIGZString& destination);
	void AppendLineFormatted(cIGZString& destination, const char* const format, ...);

	bool SetLocalizedStringValue(
		uint32_t ltextGroup,
		uint32_t ltextInstance,
		cIGZString& destination);
}