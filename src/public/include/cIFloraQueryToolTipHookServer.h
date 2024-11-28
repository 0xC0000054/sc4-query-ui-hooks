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
#include "cIGZUnknown.h"

class cIFloraQueryCustomToolTipHookTarget;
class cIQueryToolTipAppendTextHookTarget;

static const uint32_t GZCLSID_cIFloraQueryToolTipHookServer = 0x7DE0764D;
static const uint32_t GZIID_cIFloraQueryToolTipHookServer = 0x7AD86A4B;

class cIFloraQueryToolTipHookServer : public cIGZUnknown
{
public:
	virtual bool AddNotification(cIFloraQueryCustomToolTipHookTarget* target) = 0;
	virtual bool RemoveNotification(cIFloraQueryCustomToolTipHookTarget* target) = 0;

	virtual bool AddNotification(cIQueryToolTipAppendTextHookTarget* target) = 0;
	virtual bool RemoveNotification(cIQueryToolTipAppendTextHookTarget* target) = 0;
};