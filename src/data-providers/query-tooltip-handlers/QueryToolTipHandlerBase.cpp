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

#include "QueryToolTipHandlerBase.h"

void QueryToolTipHandlerBase::PostAppInit(cIGZCOM* pCOM)
{
}

void QueryToolTipHandlerBase::PreAppShutdown(cIGZCOM* pCOM)
{
}

QueryToolTipHandlerBase::QueryToolTipHandlerBase()
	: refCount(0)
{
}

bool QueryToolTipHandlerBase::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIGZUnknown)
	{
		*ppvObj = static_cast<cIGZUnknown*>(this);
		AddRef();

		return true;
	}

	return false;
}

uint32_t QueryToolTipHandlerBase::AddRef()
{
	return ++refCount;
}

uint32_t QueryToolTipHandlerBase::Release()
{
	if (refCount > 0)
	{
		--refCount;
	}

	return refCount;
}
