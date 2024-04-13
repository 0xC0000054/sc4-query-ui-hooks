#pragma once
#include "cIGZUnknown.h"

class cGZPersistResourceKey;

class cIGZPersistDBRecord : public cIGZUnknown
{
public:

	virtual void GetKey(cGZPersistResourceKey& key) = 0;
	virtual uint16_t GetAccessFlags() = 0;
	virtual bool Close() = 0;

	virtual bool GetFieldVoid(void* destination, uint32_t byteCount) = 0;
	virtual bool SetFieldVoid(const void* source, uint32_t byteCount) = 0;

	virtual uint32_t GetSize() = 0;
	virtual bool SetSize(uint32_t size) = 0;

	virtual uint32_t GetPosition() = 0;
	virtual bool SeekAbsolute(uint32_t position) = 0;
	virtual bool SeekRelative(int32_t position) = 0;
};