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
#include "QueryToolHelpers.h"
#include <cstdarg>
#include <cstdio>

namespace
{
	typedef void*(__thiscall* RZString_Append_PChar_Length)(void* thisPtr, const char* data, uint32_t length);
	typedef int32_t(__cdecl* RZString_Sprintf)(void* thisPtr, const char* format, ...);

	static const RZString_Append_PChar_Length RealRZStringAppend_PChar_Length = reinterpret_cast<RZString_Append_PChar_Length>(0x90F053);
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
			uint8_t cellMoisture = spWeatherSimulator->GetMoistureValue(x, z);

			result = RealRZStringSprintf(
				rzStringThisPtr,
				"x=%f y=%f z=%f\ncell x=%d cell z=%d\ncell moisture=%d",
				x,
				y,
				z,
				cellX,
				cellZ,
				cellMoisture);

			if (result && QueryToolHelpers::IsDebugQueryEnabled())
			{
				float humidity = spWeatherSimulator->GetHumidity(x, z);

				cS3DVector2 ambientWindDirection;

				float ambientWindSpeed = spWeatherSimulator->GetWindAtCell(cellX, cellZ, ambientWindDirection);

				char buffer[1024]{};

				int length = std::snprintf(
					buffer,
					sizeof(buffer),
					" humidity=%f\nambient wind:\nspeed=%f\ndirection[0]=%f direction[1]=%f",
					humidity,
					ambientWindSpeed,
					ambientWindDirection.fX,
					ambientWindDirection.fY);

				if (length > 0)
				{
					RealRZStringAppend_PChar_Length(rzStringThisPtr, buffer, static_cast<uint32_t>(length));
				}
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
