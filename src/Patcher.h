///////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-query-ui-hooks, a DLL Plugin for SimCity 4 that
// extends the query UI.
//
// Copyright (c) 2024, 2025 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include <cstdint>

#ifdef __clang__
#define NAKED_FUN __attribute__((naked))
#else
#define NAKED_FUN __declspec(naked)
#endif

namespace Patcher
{
	void InstallJump(uintptr_t address, uintptr_t destination);

	void InstallCallHook(uintptr_t address, void* pfnFunc);
}
