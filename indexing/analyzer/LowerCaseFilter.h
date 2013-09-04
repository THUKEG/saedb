#pragma once
#include <cctype>
#include "TokenFilter.h"
#include "Character.h"

using std::string;

class LowerCaseFilter :
	public TokenFilter
{
public:
	LowerCaseFilter(void) { };
	LowerCaseFilter(TokenStream* in) : TokenFilter(in) { }
	~LowerCaseFilter(void) = default;

	bool next(Token& token){
		bool ans = input->next(token);
		if (ans)
		{
			Character::toLowerCase(token.getWsTermText());
		}
		return ans;
	}
};

