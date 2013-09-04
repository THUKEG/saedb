#pragma once
#include"Tokenizer.h"

class FilterBase
{
public:

	FilterBase()
	{
	}

	virtual ~FilterBase(void)
	{
	}


private:
	Tokenizer* tokenizer;
};

