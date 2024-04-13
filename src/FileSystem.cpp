///////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-query-ui-hooks, a DLL Plugin for SimCity 4 that
// provides more data for the query UI.
//
// Copyright (c) 2024 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
///////////////////////////////////////////////////////////////////////////////

#include "FileSystem.h"
#include <Windows.h>
#include "wil/resource.h"
#include "wil/win32_helpers.h"

namespace
{
	std::filesystem::path GetDllFolderPathCore()
	{
		wil::unique_cotaskmem_string modulePath = wil::GetModuleFileNameW(wil::GetModuleInstanceHandle());

		std::filesystem::path temp(modulePath.get());

		return temp.parent_path();
	}
}

std::filesystem::path FileSystem::GetDllFolderPath()
{
	static std::filesystem::path path = GetDllFolderPathCore();

	return path;
}
