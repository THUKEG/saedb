#pragma once
#include <iostream>
#include "CharTokenizer.h"
#include "Character.h"

using std::string;

class LetterTokenizer :
	public CharTokenizer
{
public:
	LetterTokenizer(void)
	{
	}
	~LetterTokenizer(void)
	{
	}
	/** Construct a new LetterTokenizer. */
	LetterTokenizer(const string& in):CharTokenizer(in) {
		test = Character();
	}
	//string input;
protected:
	/** Collects only characters which satisfy
		* {@link Character#isLetter(char)}.*/
	bool isTokenChar(char c) {
		//return char::IsLetter(c); //TODO...
		/*if (((c>='A')&&(c<='Z')) || ((c>='a')&&(c<='z'))) 
			return true; 
		else
			return false;
			*/
		//Character test = Character();
		return test.isLetter(c);
	}
	Character test;
	/*virtual wchar_t normalize(wchar_t c) {
		//cout<<"normalize in letterToken..."<<endl;
		//return c;
		//Character test = Character();
		return test.toLowerCase(c);
	}*/
};

