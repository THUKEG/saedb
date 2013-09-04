#pragma once
#include "TokenStream.h"

class Tokenizer :
	public TokenStream
{
public:
	Tokenizer(void)
	{
	}
	~Tokenizer(void)
	{
	}
	Tokenizer(string& in)
	:input(in)
	{
	}
protected:
	string input;
};

