#pragma once
#include "TokenStream.h"
class TokenFilter :
	public TokenStream
{
public:
	TokenFilter(void)
	{
	}
	virtual ~TokenFilter(void)
	{
		delete this->input;
	}
	TokenStream* input;
	TokenFilter(TokenStream* in) {
		input = in;
	}
	/*bool hasMore(){
		bool hasMoreAns = input->hasMore();
		return hasMoreAns;
	}*/
	bool next(Token& token){
		return input->next(token);
	}
	
	void reset() {
            input->reset();
        }
	
};

