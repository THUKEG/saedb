#pragma once
#include <iostream>
#include "Token.h"

class TokenStream
{
public:
	TokenStream(void)
	{
	}

	virtual ~TokenStream(void)
	{
	}

	virtual bool next(Token & token){
		return true;
	}

	/** Resets this stream to the beginning. This is an
     *  optional operation, so subclasses may or may not
     *  implement this method. Reset() is not needed for
     *  the standard indexing process. However, if the Tokens
     *  of a TokenStream are intended to be consumed more than
     *  once, it is neccessary to implement reset().
     */
	virtual void reset(){};
};

