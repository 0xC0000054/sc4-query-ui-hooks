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

#include "QueryToolTipDllDirector.h"
#include "Logger.h"
#include "cIGZApp.h"
#include "cIGZCOM.h"
#include "cIGZFrameWork.h"
#include "cIGZMessage2Standard.h"
#include "cIGZMessageServer2.h"
#include "cIGZString.h"
#include "cISC4City.h"
#include "cRZBaseString.h"
#include <GZServPtrs.h>

#include <array>

using namespace std::string_view_literals;

static constexpr uint32_t kSC4MessagePostCityInit = 0x26D31EC1;
static constexpr uint32_t kSC4MessagePreCityShutdown = 0x26D31EC2;

static constexpr std::array<uint32_t, 2> RequiredNotifications =
{
	kSC4MessagePostCityInit,
	kSC4MessagePreCityShutdown
};

static constexpr uint32_t kNetworkQueryDllDirectorID = 0x551FA427;

QueryToolTipDllDirector::QueryToolTipDllDirector()
{
}

bool QueryToolTipDllDirector::QueryInterface(uint32_t riid, void** ppvObj)
{
	if (riid == GZIID_cINetworkQueryCustomToolTipHookTarget)
	{
		return networkToolTipHandler.QueryInterface(riid, ppvObj);
	}

	return cRZMessage2COMDirector::QueryInterface(riid, ppvObj);
}

uint32_t QueryToolTipDllDirector::GetDirectorID() const
{
	return kNetworkQueryDllDirectorID;
}

bool QueryToolTipDllDirector::OnStart(cIGZCOM* pCOM)
{
	cIGZFrameWork* const pFramework = pCOM->FrameWork();

	if (pFramework->GetState() < cIGZFrameWork::kStatePreAppInit)
	{
		pFramework->AddHook(this);
	}
	else
	{
		PreAppInit();
	}

	return true;
}

bool QueryToolTipDllDirector::PostAppInit()
{
	cIGZMessageServer2Ptr pMsgServ;

	if (pMsgServ)
	{
		for (uint32_t messageID : RequiredNotifications)
		{
			pMsgServ->AddNotification(this, messageID);
		}
	}

	return true;
}

bool QueryToolTipDllDirector::DoMessage(cIGZMessage2* pMsg)
{
	cIGZMessage2Standard* pStandardMsg = static_cast<cIGZMessage2Standard*>(pMsg);

	switch (pMsg->GetType())
	{
	case kSC4MessagePostCityInit:
		PostCityInit(pStandardMsg);
		break;
	case kSC4MessagePreCityShutdown:
		PreCityShutdown(pStandardMsg);
		break;
	}

	return true;
}

void QueryToolTipDllDirector::PostCityInit(cIGZMessage2Standard* pStandardMsg)
{
	networkToolTipHandler.PostCityInit(pStandardMsg, mpCOM);
}

void QueryToolTipDllDirector::PreCityShutdown(cIGZMessage2Standard* pStandardMsg)
{
	networkToolTipHandler.PreCityShutdown(pStandardMsg, mpCOM);
}
