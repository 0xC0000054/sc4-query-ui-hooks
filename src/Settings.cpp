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

#include "Settings.h"
#include "FileSystem.h"
#include "IniReader.h"
#include "Logger.h"
#include <fstream>

Settings::Settings()
	: enableOccupantQuerySounds(true),
	  logBuildingPluginPath(false)
{
}

bool Settings::EnableOccupantQuerySounds() const
{
	return enableOccupantQuerySounds;
}

bool Settings::LogBuildingPluginPath() const
{
	return logBuildingPluginPath;
}

void Settings::Load()
{
	Logger& logger = Logger::GetInstance();

	try
	{
		std::ifstream stream(FileSystem::GetConfigFilePath(), std::ifstream::in);

		if (stream)
		{
			IniReader iniReader(stream);

			const auto& queryUIHooksSection = iniReader.get_section("QueryUIHooks");

			enableOccupantQuerySounds = queryUIHooksSection.get_converted_value<bool>("EnableOccupantQuerySounds");
			logBuildingPluginPath = queryUIHooksSection.get_converted_value<bool>("LogBuildingPluginPath");
		}
		else
		{
			logger.WriteLine(LogLevel::Error, "Failed to open the settings file.");
		}
	}
	catch (const std::exception& e)
	{
		logger.WriteLineFormatted(
			LogLevel::Error,
			"Error reading the settings file: %s",
			e.what());
	}
}
