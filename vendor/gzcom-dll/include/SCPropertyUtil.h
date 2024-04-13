#pragma once
#include "cISCPropertyHolder.h"

namespace SCPropertyUtil
{
	bool GetPropertyValue(const cISCPropertyHolder* propertyHolder, uint32_t id, uint32_t& value);
}