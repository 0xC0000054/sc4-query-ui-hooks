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

#include "PropQueryToolTipHookServer.h"
#include "cIPropQueryCustomToolTipHookTarget.h"
#include "cIQueryToolTipAppendTextHookTarget.h"
#include "cRZBaseString.h"
#include "GZStringUtil.h"

PropQueryToolTipHookServer::PropQueryToolTipHookServer()
	: refCount(0)
{
}

bool PropQueryToolTipHookServer::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIPropQueryToolTipHookServer)
	{
		*ppvObj = static_cast<cIPropQueryToolTipHookServer*>(this);
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

uint32_t PropQueryToolTipHookServer::AddRef()
{
	return ++refCount;
}

uint32_t PropQueryToolTipHookServer::Release()
{
	if (refCount > 0)
	{
		--refCount;
	}

	return refCount;
}

bool PropQueryToolTipHookServer::HasCustomToolTipSubscribers() const
{
	return customToolTipHookSubscribers.size() > 0;
}

bool PropQueryToolTipHookServer::SendCustomToolTipMessage(
	cISC4Occupant* const occupant,
	bool debugQuery,
	cIGZString& title,
	cIGZString& text)
{
	bool tooltipHandled = false;

	size_t subscriberCount = customToolTipHookSubscribers.size();
	if (subscriberCount > 0)
	{
		for (cIPropQueryCustomToolTipHookTarget* pTarget : customToolTipHookSubscribers)
		{
			cIPropQueryCustomToolTipHookTarget* localTarget = pTarget;

			if (localTarget)
			{
				if (localTarget->ProcessToolTip(
					occupant,
					debugQuery,
					title,
					text))
				{
					tooltipHandled = true;
					break;
				}
			}
		}
	}

	return tooltipHandled;
}

bool PropQueryToolTipHookServer::HasAppendToolTipSubscribers() const
{
	return appendToolTipHookSubscribers.size() > 0;
}

void PropQueryToolTipHookServer::SendAppendToolTipMessage(cISC4Occupant* const occupant, bool debugQuery, cIGZString& destination)
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

bool PropQueryToolTipHookServer::AddNotification(cIPropQueryCustomToolTipHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = customToolTipHookSubscribers.emplace(target).second;
	}

	return result;
}

bool PropQueryToolTipHookServer::RemoveNotification(cIPropQueryCustomToolTipHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = customToolTipHookSubscribers.erase(target) == 1;
	}

	return result;
}

bool PropQueryToolTipHookServer::AddNotification(cIQueryToolTipAppendTextHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = appendToolTipHookSubscribers.emplace(target).second;
	}

	return result;
}

bool PropQueryToolTipHookServer::RemoveNotification(cIQueryToolTipAppendTextHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = appendToolTipHookSubscribers.erase(target) == 1;
	}

	return result;
}
