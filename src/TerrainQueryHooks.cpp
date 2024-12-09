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

#include "TerrainQueryHooks.h"
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

	typedef int32_t(__cdecl* RZString_Sprintf)(void* thisPtr, const char* format, ...);

	static const RZString_Sprintf RealRZStringSprintf = reinterpret_cast<RZString_Sprintf>(0x90F574);

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

		if (spWeatherSimulator)
		{
			// Pressing the Alt key will show the humidity and ambient wind information, this data appears
			// to be for the entire tile instead of varying per-cell.
			// It is not shown in the advanced query to reduce the amount of text that is shown in that mode.

			if (IsKeyDownNow(VK_MENU) && !IsKeyDownNow(VK_SHIFT) && !IsKeyDownNow(VK_CONTROL))
			{
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
			else
			{
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
