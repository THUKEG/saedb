#pragma once
#include "StemFilter.h"
#include <unordered_set>

using std::unordered_set;

class StopFilter :
	public TokenFilter
{
public:
	StopFilter(void) = default;
	~StopFilter(void) = default;
	StopFilter(TokenStream* in);
	bool next(Token&);

private:
	static unordered_set<string> stopTokens;
};

