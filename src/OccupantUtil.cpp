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

#include "OccupantUtil.h"
#include "cIGZVariant.h"
#include "cISCProperty.h"
#include "cISCPropertyHolder.h"
#include "cISC4Occupant.h"
#include "cRZAutoRefCount.h"
#include "StringResourceKey.h"
#include "StringResourceManager.h"

namespace
{
	bool GetUserVisibleNameKey(cISC4Occupant* pOccupant, StringResourceKey& key)
	{
		bool result = false;

		if (pOccupant)
		{
			cISCPropertyHolder* propertyHolder = pOccupant->AsPropertyHolder();

			constexpr uint32_t kUserVisibleName = 0x8A416A99;

			cISCProperty* userVisibleName = propertyHolder->GetProperty(kUserVisibleName);

			if (userVisibleName)
			{
				const cIGZVariant* propertyValue = userVisibleName->GetPropertyValue();

				if (propertyValue->GetType() == cIGZVariant::Type::Uint32Array
					&& propertyValue->GetCount() == 3)
				{
					const uint32_t* pTGI = propertyValue->RefUint32();

					uint32_t group = pTGI[1];
					uint32_t instance = pTGI[2];

					key.groupID = group;
					key.instanceID = instance;
					result = true;
				}
			}
		}

		return result;
	}
}

bool OccupantUtil::GetUserVisibleName(cISC4Occupant* pOccupant, cRZAutoRefCount<cIGZString>& name)
{
	bool result = false;

	StringResourceKey key;

	if (GetUserVisibleNameKey(pOccupant, key))
	{
		result = StringResourceManager::GetLocalizedString(key, name.AsPPObj());
	}

	return result;
}
