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

#pragma once
#include "cIGZUnknown.h"

class cINetworkQueryCustomToolTipHookTarget;
class cIQueryToolTipAppendTextHookTarget;

static const uint32_t GZCLSID_cINetworkQueryToolTipHookServer = 0x32A9898E;
static const uint32_t GZIID_cINetworkQueryToolTipHookServer = 0x384B6D29;

class cINetworkQueryToolTipHookServer : public cIGZUnknown
{
public:
	virtual bool AddNotification(cINetworkQueryCustomToolTipHookTarget* target) = 0;
	virtual bool RemoveNotification(cINetworkQueryCustomToolTipHookTarget* target) = 0;

	virtual bool AddNotification(cIQueryToolTipAppendTextHookTarget* target) = 0;
	virtual bool RemoveNotification(cIQueryToolTipAppendTextHookTarget* target) = 0;
};