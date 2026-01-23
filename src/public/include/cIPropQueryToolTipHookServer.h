///////////////////////////////////////////////////////////////////////////////
//
// This file is part of sc4-query-ui-hooks, a DLL Plugin for SimCity 4 that
// extends the query UI.
//
// Copyright (c) 2024, 2025, 2026 Nicholas Hayes
//
// This file is licensed under terms of the MIT License.
// See LICENSE.txt for more information.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "cIGZUnknown.h"

class cIPropQueryCustomToolTipHookTarget;
class cIQueryToolTipAppendTextHookTarget;

static const uint32_t GZCLSID_cIPropQueryToolTipHookServer = 0x8D73886D;
static const uint32_t GZIID_cIPropQueryToolTipHookServer = 0x8D73886F;

class cIPropQueryToolTipHookServer : public cIGZUnknown
{
public:
	virtual bool AddNotification(cIPropQueryCustomToolTipHookTarget* target) = 0;
	virtual bool RemoveNotification(cIPropQueryCustomToolTipHookTarget* target) = 0;

	virtual bool AddNotification(cIQueryToolTipAppendTextHookTarget* target) = 0;
	virtual bool RemoveNotification(cIQueryToolTipAppendTextHookTarget* target) = 0;
};