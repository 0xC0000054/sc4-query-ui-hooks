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

#include "TerrainQueryHooks.h"
#include "cISC4FlammabilitySimulator.h"
#include "cISC4LandValueSimulator.h"
#include "cISC4PollutionSimulator.h"
#include "cISC4SimGrid.h"
#include "cISC4WeatherSimulator.h"
#include "cS3DVector2.h"
#include "GlobalSC4InterfacePointers.h"
#include "Patcher.h"
#include <cstdarg>
#include <Windows.h>

namespace
{
	bool IsKeyDownNow(int vKey)
	{
		return (GetAsyncKeyState(vKey) & 0x8000) != 0;
	}

	enum class ModifierKeys : uint32_t
	{
		None = 0,
		Control = 1 << 0,
		Alt = 1 << 1,
		Shift = 1 << 2,
		ControlAltShift = Control | Alt | Shift
	};
	DEFINE_ENUM_FLAG_OPERATORS(ModifierKeys);

	ModifierKeys GetActiveModifierKeys()
	{
		ModifierKeys modifiers = ModifierKeys::None;

		if (IsKeyDownNow(VK_CONTROL))
		{
			modifiers |= ModifierKeys::Control;
		}

		if (IsKeyDownNow(VK_MENU))
		{
			modifiers |= ModifierKeys::Alt;
		}

		if (IsKeyDownNow(VK_SHIFT))
		{
			modifiers |= ModifierKeys::Shift;
		}

		return modifiers;
	}

	typedef int32_t(__cdecl* RZString_Sprintf)(void* thisPtr, const char* format, ...);

	static const RZString_Sprintf RealRZStringSprintf = reinterpret_cast<RZString_Sprintf>(0x90F574);

	template<typename T> T GetSimGridCellValue(const cISC4SimGrid<T>* grid, int32_t x, int32_t z)
	{
		return grid->GetCellValue(x, z);
	}

	int32_t HookedTerrainQuerySprintf(void* rzStringThisPtr, const char* format, ...)
	{
		// Unpack the arguments that were passed to the function.

		va_list args;
		va_start(args, format);

		float x = static_cast<float>(va_arg(args, double));
		float y = static_cast<float>(va_arg(args, double));
		float z = static_cast<float>(va_arg(args, double));
		int32_t cellX = va_arg(args, int32_t);
		int32_t cellZ = va_arg(args, int32_t);

		va_end(args);

		int32_t result = 0;

		if (spFlammabilitySimulator && spLandValueSimulator && spPollutionSimulator && spWeatherSimulator)
		{
			const ModifierKeys modifiers = GetActiveModifierKeys();

			if (modifiers == ModifierKeys::None
				|| (modifiers & ModifierKeys::ControlAltShift) == ModifierKeys::ControlAltShift)
			{
				// Show the standard query with the cell moisture added.
				// This info is also appended to the end of the Control + Alt + Shift advanced/debug
				// queries, so it need to be fairly short.

				uint8_t cellMoisture = spWeatherSimulator->GetMoistureValue(x, z);

				result = RealRZStringSprintf(
					rzStringThisPtr,
					"x=%f y=%f z=%f\ncell x=%d cell z=%d cell moisture=%d",
					x,
					y,
					z,
					cellX,
					cellZ,
					cellMoisture);
			}
			else if ((modifiers & ModifierKeys::ControlAltShift) == ModifierKeys::Alt)
			{
				// Pressing the Alt key will show the humidity and ambient wind information, this data appears
				// to be for the entire tile instead of varying per-cell.

				float humidity = spWeatherSimulator->GetHumidity(x, z);

				cS3DVector2 ambientWindDirection;

				float ambientWindSpeed = spWeatherSimulator->GetWindAtCell(cellX, cellZ, ambientWindDirection);

				result = RealRZStringSprintf(
					rzStringThisPtr,
					"x=%f y=%f z=%f\ncell x=%d cell z=%d cell humidity=%f\n"
					"ambient wind speed=%f\nambient wind direction[0]=%f\nambient wind direction[1]=%f",
					x,
					y,
					z,
					cellX,
					cellZ,
					humidity,
					ambientWindSpeed,
					ambientWindDirection.fX,
					ambientWindDirection.fY);
			}
			else if ((modifiers & ModifierKeys::ControlAltShift) == ModifierKeys::Control)
			{
				// Pressing the Control key will show the flammability, land value, and pollution.

				uint8_t flammability = GetSimGridCellValue(
					spFlammabilitySimulator->GetFlammabilityGrid(),
					cellX,
					cellZ);

				// Our land value fields use the same order as SC4's advanced/debug query.

				uint8_t intrinsicLandValue = GetSimGridCellValue(
					spLandValueSimulator->GetIntrinsicLandValueMap(),
					cellX,
					cellZ);
				uint8_t totalLandValue = spLandValueSimulator->GetLandValue(cellX, cellZ);
				const char* landValueDescription = "Unknown";

				switch (spLandValueSimulator->GetLandValueType(cellX, cellZ))
				{
				case cISC4LandValueSimulator::LandValueType::Low:
					landValueDescription = "Low";
					break;
				case cISC4LandValueSimulator::LandValueType::Medium:
					landValueDescription = "Medium";
					break;
				case cISC4LandValueSimulator::LandValueType::High:
					landValueDescription = "High";
					break;
				}

				int32_t airPollution = 0;
				int32_t waterPollution = 0;
				int32_t garbagePollution = 0;

				spPollutionSimulator->GetAirValue(cellX, cellZ, airPollution);
				spPollutionSimulator->GetWaterValue(cellX, cellZ, waterPollution);
				spPollutionSimulator->GetGarbageValue(cellX, cellZ, garbagePollution);
				bool radioactive = spPollutionSimulator->IsRadioactive(cellX, cellZ);

				result = RealRZStringSprintf(
					rzStringThisPtr,
					"x=%f y=%f z=%f\ncell x=%d cell z=%d flam=%d\n"
					"land value %d:%d (%s) pollution:\n"
					"air=%d water=%d garbage=%d rad?=%d",
					x,
					y,
					z,
					cellX,
					cellZ,
					flammability,
					intrinsicLandValue,
					totalLandValue,
					landValueDescription,
					airPollution,
					waterPollution,
					garbagePollution,
					radioactive);
			}
		}
		else
		{
			result = RealRZStringSprintf(
				rzStringThisPtr,
				"x=%f y=%f z=%f\ncell x=%d cell z=%d",
				x,
				y,
				z,
				cellX,
				cellZ);
		}

		return result;
	}
}

void TerrainQueryHooks::Install()
{
	Patcher::InstallCallHook(0x4D7335, &HookedTerrainQuerySprintf);
}
