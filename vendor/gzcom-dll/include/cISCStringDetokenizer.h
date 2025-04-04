#pragma once
#include "cIGZUnknown.h"

class cGZPersistResourceKey;
class cIGZString;

class cISCStringDetokenizer : public cIGZUnknown
{
public:
	virtual bool Init() = 0;
	virtual bool Shutdown() = 0;

	virtual void SetTokenReplacementValue(cIGZString const& token, cIGZString const& replacementValue) = 0;

	/**
	 * @brief The callback method that processes the token.
	 * @param token The token to process.
	 * @param outValue The output value for the token's result.
	 * @param pContext An optional state value that is associated with the callback.
	 * @returns True if the token was processed by the callback; otherwise, false to forward it to other callbacks.
	 */
	typedef bool (*TokenReplacementCallback)(cIGZString const& token, cIGZString& outValue, void* pContext);

	virtual void SetTokenReplacementMethod(cIGZString const& token, TokenReplacementCallback pCallback, void* pContext) = 0;
	virtual void AddUnknownTokenReplacementMethod(TokenReplacementCallback pCallback, void* pContext, bool add) = 0;

	virtual bool Detokenize(cIGZString const& tokenizedValue, cIGZString& output) = 0;
	virtual bool Detokenize(cGZPersistResourceKey const& key, cIGZString& output) = 0;

	virtual void SetHTMLCleanMode(bool value) = 0;
	virtual bool GetHTMLCleanMode() = 0;
};