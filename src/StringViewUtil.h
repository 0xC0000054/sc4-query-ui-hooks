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
#include <string_view>
#include <vector>

namespace StringViewUtil
{
	bool EqualsIgnoreCase(
		const std::string_view& lhs,
		const std::string_view& rhs);

	bool StartsWithIgnoreCase(
		const std::string_view& lhs,
		const std::string_view& rhs);

	void Split(
		const std::string_view& input,
		std::string_view::value_type delim,
		std::vector<std::string_view>& results);

	/**
	 * @brief Removes the specified number of characters from the start of the input.
	 * @param input The input.
	 * @param length The number of characters to remove.
	 * @return A trimmed copy of the input, or an empty std::string_view if the
	 * input contained fewer than length characters.
	 */
	std::string_view RemoveLeft(const std::string_view& input, size_t length);

	std::string_view TrimQuotes(const std::string_view& input);
	std::string_view TrimWhiteSpace(const std::string_view& input);

	bool TryParse(const std::string_view& input, long& outValue);
	bool TryParse(const std::string_view& input, uint32_t& outValue);
}