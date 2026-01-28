/*
 * This file is part of sc4-query-ui-hooks, a DLL Plugin for SimCity 4 that
 * extends the query UI.
 *
 * Copyright (C) 2024, 2025, 2026 Nicholas Hayes
 *
 * sc4-query-ui-hooks is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * sc4-query-ui-hooks is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with sc4-query-ui-hooks.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "LuaHelper.h"
#include "cRZBaseString.h"

void LuaHelper::SetResultFromIGZVariant(cISCLua* pLua, const cIGZVariant* pVariant)
{
	if (!pVariant)
	{
		pLua->PushNil();
		return;
	}

	switch (static_cast<cIGZVariant::Type>(pVariant->GetType()))
	{
	case cIGZVariant::Type::Bool:
		PushValue(pLua, pVariant->GetValBool());
		break;
	case cIGZVariant::Type::BoolArray:
		CreateLuaArray(pLua, pVariant->RefBool(), pVariant->GetCount());
		break;
	case cIGZVariant::Type::Uint8:
		PushValue(pLua, pVariant->GetValUint8());
		break;
	case cIGZVariant::Type::Uint8Array:
		CreateLuaArray(pLua, pVariant->RefUint8(), pVariant->GetCount());
		break;
	case cIGZVariant::Type::Sint8:
		PushValue(pLua, pVariant->GetValSint8());
		break;
	case cIGZVariant::Type::Sint8Array:
		CreateLuaArray(pLua, pVariant->RefSint8(), pVariant->GetCount());
		break;
	case cIGZVariant::Type::Uint16:
		PushValue(pLua, pVariant->GetValUint16());
		break;
	case cIGZVariant::Type::Uint16Array:
		CreateLuaArray(pLua, pVariant->RefUint16(), pVariant->GetCount());
		break;
	case cIGZVariant::Type::Sint16:
		PushValue(pLua, pVariant->GetValSint16());
		break;
	case cIGZVariant::Type::Sint16Array:
		CreateLuaArray(pLua, pVariant->RefSint16(), pVariant->GetCount());
		break;
	case cIGZVariant::Type::Uint32:
		PushValue(pLua, pVariant->GetValUint32());
		break;
	case cIGZVariant::Type::Uint32Array:
		CreateLuaArray(pLua, pVariant->RefUint32(), pVariant->GetCount());
		break;
	case cIGZVariant::Type::Sint32:
		PushValue(pLua, pVariant->GetValSint32());
		break;
	case cIGZVariant::Type::Sint32Array:
		CreateLuaArray(pLua, pVariant->RefSint32(), pVariant->GetCount());
		break;
	case cIGZVariant::Type::Uint64:
		PushValue(pLua, pVariant->GetValUint64());
		break;
	case cIGZVariant::Type::Uint64Array:
		CreateLuaArray(pLua, pVariant->RefUint64(), pVariant->GetCount());
		break;
	case cIGZVariant::Type::Sint64:
		PushValue(pLua, pVariant->GetValSint64());
		break;
	case cIGZVariant::Type::Sint64Array:
		CreateLuaArray(pLua, pVariant->RefSint64(), pVariant->GetCount());
		break;
	case cIGZVariant::Type::Float32:
		PushValue(pLua, pVariant->GetValFloat32());
		break;
	case cIGZVariant::Type::Float32Array:
		CreateLuaArray(pLua, pVariant->RefFloat32(), pVariant->GetCount());
		break;
	case cIGZVariant::Type::Float64:
		PushValue(pLua, pVariant->GetValFloat64());
		break;
	case cIGZVariant::Type::Float64Array:
		CreateLuaArray(pLua, pVariant->RefFloat64(), pVariant->GetCount());
		break;
	default:
		cRZBaseString asString;
		pVariant->GetValString(asString);

		pLua->PushLString(asString.Data(), asString.Strlen());
		break;
	}
}
