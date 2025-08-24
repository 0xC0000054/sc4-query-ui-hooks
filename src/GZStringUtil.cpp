/*
 * This file is part of sc4-query-ui-hooks, a DLL Plugin for SimCity 4 that
 * extends the query UI.
 *
 * Copyright (C) 2024, 2025 Nicholas Hayes
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

#include "GZStringUtil.h"
#include "cIGZString.h"
#include "cRZAutoRefCount.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"
#include <cstdarg>
#include <memory>

namespace
{
	bool EndsWithNewLine(const cIGZString& line)
	{
		bool result = false;

		uint32_t length = line.Strlen();

		if (length > 0)
		{
			const char* const chars = line.ToChar();

			result = chars[length - 1] == '\n';
		}

		return result;
	}
}

void GZStringUtil::AppendLine(const std::string_view& line, cIGZString& destination)
{
	destination.Append(line.data(), line.size());

	if (!line.ends_with('\n'))
	{
		destination.Append("\n", 1);
	}
}

void GZStringUtil::AppendLine(const cIGZString& line, cIGZString& destination)
{
	destination.Append(line);

	if (!EndsWithNewLine(line))
	{
		destination.Append("\n", 1);
	}
}

void GZStringUtil::AppendLineFormatted(cIGZString& destination, const char* const format, ...)
{
	va_list args;
	va_start(args, format);

	va_list argsCopy;
	va_copy(argsCopy, args);

	int formattedStringLength = std::vsnprintf(nullptr, 0, format, argsCopy);

	va_end(argsCopy);

	if (formattedStringLength > 0)
	{
		size_t formattedStringLengthWithNull = static_cast<size_t>(formattedStringLength) + 1;

		constexpr size_t stackBufferSize = 1024;

		if (formattedStringLengthWithNull >= stackBufferSize)
		{
			std::unique_ptr<char[]> buffer = std::make_unique_for_overwrite<char[]>(formattedStringLengthWithNull);

			std::vsnprintf(buffer.get(), formattedStringLengthWithNull, format, args);

			AppendLine(buffer.get(), destination);
		}
		else
		{
			char buffer[stackBufferSize]{};

			std::vsnprintf(buffer, stackBufferSize, format, args);

			AppendLine(buffer, destination);
		}
	}

	va_end(args);
}

bool GZStringUtil::SetLocalizedStringValue(
	uint32_t ltextGroup,
	uint32_t ltextInstance,
	cIGZString& destination)
{
	bool result = false;

	StringResourceKey key(ltextGroup, ltextInstance);

	cRZAutoRefCount<cIGZString> localizedString;

	if (StringResourceManager::GetLocalizedString(key, localizedString))
	{
		destination.Copy(*localizedString);
		result = true;
	}

	return result;
}
