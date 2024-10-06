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

#include "Patcher.h"
#include <Windows.h>
#include "wil/result.h"

void Patcher::InstallCallHook(uintptr_t targetAddress, void* pfnFunc)
{
	// Allow the executable memory to be written to.
	DWORD oldProtect = 0;
	THROW_IF_WIN32_BOOL_FALSE(VirtualProtect(
		reinterpret_cast<LPVOID>(targetAddress),
		5,
		PAGE_EXECUTE_READWRITE,
		&oldProtect));

	// Patch the memory at the specified address.
	*((uint8_t*)targetAddress) = 0xE8;
	*((uintptr_t*)(targetAddress + 1)) = ((uintptr_t)pfnFunc) - targetAddress - 5;
}
