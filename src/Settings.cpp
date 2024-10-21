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

#include "Settings.h"
#include "FileSystem.h"
#include "Logger.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/ini_parser.hpp"
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
			boost::property_tree::ptree tree;
			boost::property_tree::ini_parser::read_ini(stream, tree);

			enableOccupantQuerySounds = tree.get<bool>("QueryUIHooks.EnableOccupantQuerySounds");
			logBuildingPluginPath = tree.get<bool>("QueryUIHooks.LogBuildingPluginPath");
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
