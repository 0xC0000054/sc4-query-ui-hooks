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