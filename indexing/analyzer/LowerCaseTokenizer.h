#pragma once
#include "lettertokenizer.h"
class LowerCaseTokenizer :
	public LetterTokenizer
{
public:
	LowerCaseTokenizer(void)
	{
	}
	~LowerCaseTokenizer(void)
	{
	}
	LowerCaseTokenizer(string in) :LetterTokenizer(in){
    //LetterTokenizer::LetterTokenizer(in);
  }

  /** Collects only characters which satisfy
   * {@link Character#isLetter(char)}.*/
//protected:
	char normalize(char c) {
		cout<<"normalize in LowerCaseToken..."<<endl;
		return Character::toLowerCase(c);
	}
	
};

