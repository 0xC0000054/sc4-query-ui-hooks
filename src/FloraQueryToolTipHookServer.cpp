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

#include "FloraQueryToolTipHookServer.h"
#include "cIFloraQueryCustomToolTipHookTarget.h"
#include "cIQueryToolTipAppendTextHookTarget.h"
#include "cRZBaseString.h"
#include "GZStringUtil.h"

FloraQueryToolTipHookServer::FloraQueryToolTipHookServer()
	: refCount(0)
{
}

bool FloraQueryToolTipHookServer::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cIFloraQueryToolTipHookServer)
	{
		*ppvObj = static_cast<cIFloraQueryToolTipHookServer*>(this);
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

uint32_t FloraQueryToolTipHookServer::AddRef()
{
	return ++refCount;
}

uint32_t FloraQueryToolTipHookServer::Release()
{
	if (refCount > 0)
	{
		--refCount;
	}

	return refCount;
}

bool FloraQueryToolTipHookServer::HasCustomToolTipSubscribers() const
{
	return customToolTipHookSubscribers.size() > 0;
}

bool FloraQueryToolTipHookServer::SendCustomToolTipMessage(
	cISC4Occupant* const occupant,
	bool debugQuery,
	cIGZString& title,
	cIGZString& text)
{
	bool tooltipHandled = false;

	size_t subscriberCount = customToolTipHookSubscribers.size();
	if (subscriberCount > 0)
	{
		for (cIFloraQueryCustomToolTipHookTarget* pTarget : customToolTipHookSubscribers)
		{
			cIFloraQueryCustomToolTipHookTarget* localTarget = pTarget;

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

bool FloraQueryToolTipHookServer::HasAppendToolTipSubscribers() const
{
	return appendToolTipHookSubscribers.size() > 0;
}

void FloraQueryToolTipHookServer::SendAppendToolTipMessage(cISC4Occupant* const occupant, bool debugQuery, cIGZString& destination)
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

bool FloraQueryToolTipHookServer::AddNotification(cIFloraQueryCustomToolTipHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = customToolTipHookSubscribers.emplace(target).second;
	}

	return result;
}

bool FloraQueryToolTipHookServer::RemoveNotification(cIFloraQueryCustomToolTipHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = customToolTipHookSubscribers.erase(target) == 1;
	}

	return result;
}

bool FloraQueryToolTipHookServer::AddNotification(cIQueryToolTipAppendTextHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = appendToolTipHookSubscribers.emplace(target).second;
	}

	return result;
}

bool FloraQueryToolTipHookServer::RemoveNotification(cIQueryToolTipAppendTextHookTarget* target)
{
	bool result = false;

	if (target)
	{
		result = appendToolTipHookSubscribers.erase(target) == 1;
	}

	return result;
}
