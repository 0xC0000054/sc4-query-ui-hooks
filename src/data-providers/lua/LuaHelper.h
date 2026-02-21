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
#include "cIGZVariant.h"
#include "cISCLua.h"
#include "SafeInt.hpp"
#include <charconv>

namespace LuaHelper
{
	template <typename T>
	bool GetNumber(cISCLua* pLua, int32_t parameterIndex, T& outValue)
	{
		bool result = false;

		if (pLua)
		{
			if (pLua->IsNumber(parameterIndex))
			{
				double number = pLua->ToNumber(parameterIndex);

				// The native number format of SimCity 4's Lua 5.0 implementation is Float64/double.
				// We perform our own casting for integer types in order to get an error for values
				// that are out of range for the destination type.

				if constexpr (std::is_same_v<T, double>)
				{
					outValue = number;
					result = true;
				}
				else if constexpr (std::is_same_v<T, float>)
				{
					outValue = static_cast<float>(number);
					result = true;
				}
				else
				{
					result = SafeCast(number, outValue);
				}
			}
			else if constexpr (std::is_same_v<T, uint32_t>)
			{
				// Uint32 values can also be specified as a hexadecimal string
				// in the format 1234abcd or 0x1234abcd.
				// This makes the Lua methods more intuitive to use by
				// letting callers skip using the hex2dec function.

				if (pLua->IsString(parameterIndex))
				{
					const char* const text = pLua->ToString(parameterIndex);
					const uint32_t textLength = pLua->Strlen(parameterIndex);

					// The string must have an even number of characters.
					if (textLength > 0 && (textLength % 2) == 0)
					{
						const char* start = text;
						const char* end = text + textLength;

						if (textLength > 2 && text[0] == '0')
						{
							const char second = text[1];

							if (second == 'x' || second == 'X')
							{
								// std::from_chars can't parse hexadecimal numbers with the 0x prefix.
								start += 2;
							}
						}

						constexpr int base = 16;

						const auto fromCharsResult = std::from_chars(start, end, outValue, base);
						result = fromCharsResult.ec == std::errc{} && fromCharsResult.ptr == end;
					}
				}
			}
		}

		return result;
	}

	template <typename T>
	void PushValue(cISCLua* pLua, T value)
	{
		if constexpr (std::is_same_v<T, bool>)
		{
			pLua->PushBoolean(value);
		}
		else if constexpr (
			std::is_same_v<T, uint8_t>
			|| std::is_same_v<T, int8_t>
			|| std::is_same_v<T, uint16_t>
			|| std::is_same_v<T, int16_t>
			|| std::is_same_v<T, uint32_t>
			|| std::is_same_v<T, int32_t>
			|| std::is_same_v<T, uint64_t>
			|| std::is_same_v<T, int64_t>
			|| std::is_same_v<T, float>
			|| std::is_same_v<T, double>)
		{
			pLua->PushNumber(static_cast<double>(value));
		}
		else
		{
			static_assert(false, "Unsupported parameter type for PushValue");
		}
	}

	template <typename T>
	void CreateLuaArray(cISCLua* pLua, const T* values, uint32_t count)
	{
		if (count == 1)
		{
			PushValue(pLua, *values);
		}
		else if (count < static_cast<uint32_t>(std::numeric_limits<int32_t>::max() - 1))
		{
			pLua->NewTable();

			for (uint32_t i = 0; i < count; i++)
			{
				PushValue(pLua, values[i]);
				// Lua uses a one-based index for arrays.
				pLua->RawSetI(-2, static_cast<int32_t>(i + 1));
			}
		}
		else
		{
			pLua->PushNil();
		}
	}

	void SetResultFromIGZVariant(cISCLua* pLua, const cIGZVariant* pVariant);
}