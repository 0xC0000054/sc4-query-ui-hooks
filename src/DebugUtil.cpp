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

#include "DebugUtil.h"
#include "OccupantUtil.h"
#include <memory>
#include <Windows.h>

void DebugUtil::PrintLineToDebugOutput(const char* const line)
{
#ifdef _DEBUG
	OutputDebugStringA(line);
	OutputDebugStringA("\n");
#endif // _DEBUG
}

void DebugUtil::PrintLineToDebugOutputUtf8(const char* const line)
{
#ifdef _DEBUG
	const int wideLength = MultiByteToWideChar(CP_UTF8, 0, line, -1, nullptr, 0);

	if (wideLength > 0)
	{
		constexpr int stackBufferSize = 512;

		if (wideLength >= stackBufferSize)
		{
			std::unique_ptr<wchar_t[]> buffer = std::make_unique_for_overwrite<wchar_t[]>(wideLength);

			MultiByteToWideChar(CP_UTF8, 0, line, -1, buffer.get(), wideLength);

			OutputDebugStringW(buffer.get());
		}
		else
		{
			wchar_t buffer[stackBufferSize]{};

			MultiByteToWideChar(CP_UTF8, 0, line, -1, buffer, wideLength);

			OutputDebugStringW(buffer);
		}
	}

	OutputDebugStringA("\n");
#endif // _DEBUG
}

void DebugUtil::PrintLineToDebugOutputFormatted(const char* const format, ...)
{
#ifdef _DEBUG
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

			PrintLineToDebugOutput(buffer.get());
		}
		else
		{
			char buffer[stackBufferSize]{};

			std::vsnprintf(buffer, stackBufferSize, format, args);

			PrintLineToDebugOutput(buffer);
		}
	}

	va_end(args);
#endif // _DEBUG
}

void DebugUtil::PrintLineToDebugOutputFormattedUtf8(const char* const format, ...)
{
#ifdef _DEBUG
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

			PrintLineToDebugOutputUtf8(buffer.get());
		}
		else
		{
			char buffer[stackBufferSize]{};

			std::vsnprintf(buffer, stackBufferSize, format, args);

			PrintLineToDebugOutputUtf8(buffer);
		}
	}

	va_end(args);
#endif // _DEBUG
}

void DebugUtil::PrintOccupantNameToDebugOutput(cISC4Occupant* pOccupant)
{
#ifdef _DEBUG
	cRZAutoRefCount<cIGZString> name;

	if (OccupantUtil::GetUserVisibleName(pOccupant, name))
	{
		PrintLineToDebugOutputUtf8(name->ToChar());
	}
#endif // _DEBUG
}
