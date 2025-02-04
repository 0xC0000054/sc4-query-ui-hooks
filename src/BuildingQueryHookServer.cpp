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

#include "BuildingQueryHookServer.h"
#include "cIBuildingQueryDialogHookTarget.h"
#include "cIBuildingQueryCustomToolTipHookTarget.h"
#include "cIQueryToolTipAppendTextHookTarget.h"
#include "cRZBaseString.h"
#include "GZStringUtil.h"

BuildingQueryHookServer::BuildingQueryHookServer()
	: refCount(0)
{
}

bool BuildingQueryHookServer::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIBuildingQueryHookServer)
	{
		*ppvObj = static_cast<cIBuildingQueryHookServer*>(this);
		AddRef();
		return true;
	}
	else if (riid == GZIID_cIGZUnknown)
	{
		*ppvObj = static_cast<cIGZUnknown*>(this);
		AddRef();
		return true;
	}

	*ppvObj = nullptr;
	return false;
}

void BuildingQueryHookServer::SendBeforeQueryDialogNotifications(cISC4Occupant* pOccupant)
{
	size_t subscriberCount = dialogHookSubscribers.size();
	if (subscriberCount > 0)
	{
		for (cIBuildingQueryDialogHookTarget* pTarget : dialogHookSubscribers)
		{
			cIBuildingQueryDialogHookTarget* localTarget = pTarget;

			if (localTarget)
			{
				localTarget->BeforeDialogShown(pOccupant);
			}
		}
	}
}

void BuildingQueryHookServer::SendAfterQueryDialogNotifications(cISC4Occupant* pOccupant)
{
	size_t subscriberCount = dialogHookSubscribers.size();
	if (subscriberCount > 0)
	{
		for (cIBuildingQueryDialogHookTarget* pTarget : dialogHookSubscribers)
		{
			cIBuildingQueryDialogHookTarget* localTarget = pTarget;

			if (localTarget)
			{
				localTarget->AfterDialogShown(pOccupant);
			}
		}
	}
}

bool BuildingQueryHookServer::HasCustomToolTipSubscribers() const
{
	return customToolTipHookSubscribers.size() > 0;
}

bool BuildingQueryHookServer::SendCustomToolTipMessage(
	cISC4Occupant* const occupant,
	bool debugQuery,
	cIGZString& title,
	cIGZString& text,
	uint32_t& backgroundImageIID,
	uint32_t& meterImageIID,
	float& meterPercentage)
{
	bool tooltipHandled = false;

	size_t subscriberCount = customToolTipHookSubscribers.size();
	if (subscriberCount > 0)
	{
		for (cIBuildingQueryCustomToolTipHookTarget* pTarget : customToolTipHookSubscribers)
		{
			cIBuildingQueryCustomToolTipHookTarget* localTarget = pTarget;

			if (localTarget)
			{
				if (localTarget->ProcessToolTip(
					occupant,
					debugQuery,
					title,
					text,
					backgroundImageIID,
					meterImageIID,
					meterPercentage))
				{
					tooltipHandled = true;
					break;
				}
			}
		}
	}

	return tooltipHandled;
}

bool BuildingQueryHookServer::HasAppendToolTipSubscribers() const
{
	return appendToolTipHookSubscribers.size() > 0;
}

void BuildingQueryHookServer::SendAppendToolTipMessage(cISC4Occupant* const occupant, bool debugQuery, cIGZString& destination)
{
	size_t subscriberCount = customToolTipHookSubscribers.size();
	if (subscriberCount > 0)
	{
		for (cIQueryToolTipAppendTextHookTarget* pTarget : appendToolTipHookSubscribers)
		{
			cIQueryToolTipAppendTextHookTarget* localTarget = pTarget;

			if (localTarget)
			{
				cRZBaseString text;

				if (localTarget->AppendQueryToolTipText(
					occupant,
					debugQuery,
					text))
				{
					GZStringUtil::AppendLine(text, destination);
				}
			}
		}
	}
}

uint32_t BuildingQueryHookServer::AddRef()
{
	return ++refCount;
}

uint32_t BuildingQueryHookServer::Release()
{
	// Note that because this class is a singleton, it does not delete itself when
	// the reference count drops to zero.
	//
	// QueryDialogHooksDllDirector will hand out references to the singleton when
	// GZCOM requests an instance of the cIQueryDialogHookServer interface.

	if (refCount > 0)
	{
		--refCount;
	}

	return refCount;
}

bool BuildingQueryHookServer::AddNotification(cIBuildingQueryDialogHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = dialogHookSubscribers.emplace(target).second;
	}

	return result;
}

bool BuildingQueryHookServer::RemoveNotification(cIBuildingQueryDialogHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = dialogHookSubscribers.erase(target) == 1;
	}

	return result;
}

bool BuildingQueryHookServer::AddNotification(cIBuildingQueryCustomToolTipHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = customToolTipHookSubscribers.emplace(target).second;
	}

	return result;
}

bool BuildingQueryHookServer::RemoveNotification(cIBuildingQueryCustomToolTipHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = customToolTipHookSubscribers.erase(target) == 1;
	}

	return result;
}

bool BuildingQueryHookServer::AddNotification(cIQueryToolTipAppendTextHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = appendToolTipHookSubscribers.emplace(target).second;
	}

	return result;
}

bool BuildingQueryHookServer::RemoveNotification(cIQueryToolTipAppendTextHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = appendToolTipHookSubscribers.erase(target) == 1;
	}

	return result;
}
