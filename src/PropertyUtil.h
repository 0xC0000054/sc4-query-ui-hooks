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

#pragma once
#include "cIGZString.h"
#include "cISCPropertyHolder.h"
#include "cRZAutoRefCount.h"

namespace PropertyUtil
{
	bool GetExemplarName(
		const cISCPropertyHolder* pPropertyHolder,
		cIGZString& name);

	bool GetDisplayName(
		const cISCPropertyHolder* pPropertyHolder,
		cIGZString& name);

	bool GetUserVisibleName(
		const cISCPropertyHolder* pPropertyHolder,
		cRZAutoRefCount<cIGZString>& name);
};
