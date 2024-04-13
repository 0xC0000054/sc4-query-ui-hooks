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

class cIBuildingQueryDialogHookTarget;
class cIBuildingQueryCustomToolTipHookTarget;
class cIQueryToolTipAppendTextHookTarget;

static const uint32_t GZCLSID_cIBuildingQueryHookServer = 0x73B4DE59;
static const uint32_t GZIID_cIBuildingQueryHookServer = 0x791E53A0;

class cIBuildingQueryHookServer : public cIGZUnknown
{
public:
	virtual bool AddNotification(cIBuildingQueryDialogHookTarget* target) = 0;
	virtual bool RemoveNotification(cIBuildingQueryDialogHookTarget* target) = 0;

	virtual bool AddNotification(cIBuildingQueryCustomToolTipHookTarget* target) = 0;
	virtual bool RemoveNotification(cIBuildingQueryCustomToolTipHookTarget* target) = 0;

	virtual bool AddNotification(cIQueryToolTipAppendTextHookTarget* target) = 0;
	virtual bool RemoveNotification(cIQueryToolTipAppendTextHookTarget* target) = 0;
};