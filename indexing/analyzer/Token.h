#pragma once
#include <string>
#include "TokenUtil.h"

using std::string;

class Token
{
public:

	Token(void)
	{
	}

	~Token(void)
	{
	}

	void setTermText(const char* s, int start, int length)
	{
		text.assign(s, start, length);
	}

	void setTermText(const string& s)
	{
		text.assign(s);
	}

	string& getWsTermText()
	{
		return text;
	}

	string getTermText()
	{
		return text;
	}

	void setStartOffset(int st)
	{
		startOffset = st;
	}

	void setEndOffset(int ed)
	{
		endOffset = ed;
	}

private:
	string text;
	int startOffset, endOffset;
};

//class Token
//{
//public:
//	Token(void)
//	{
//	}
//	~Token(void)
//	{
//	}
//	Token(string text, int start, int end)
//	{
//		termText = text;
//		startOffset = start;
//		endOffset = end;
//	}
//	Token(string text, int start, int end, string& typ)
//	{
//		termText = text;
//		startOffset = start;
//		endOffset = end;
//		type = typ;
//	}
//	/** Sets the Token's term text. */
//	void setTermText(const string& text){
//		termText = text;
//	}
//
//	/** Returns the Token's term text. */
//	const string& getTermText(){ return termText; }
//
//	/** Returns this Token's starting offset, the position of the first character
//	corresponding to this token in the source text.
//
//	Note that the difference between endOffset() and startOffset() may not be
//	equal to termText.length(), as the term text may have been altered by a
//	stemmer or some other filter. */
//	int getStartOffset() { return startOffset; }
//
//	/** Returns this Token's ending offset, one greater than the position of the
//	last character corresponding to this token in the source text. */
//	int getEndOffset() { return endOffset; }
//
//	void setStartOffset(int st) { startOffset = st; }
//
//	void setEndOffset(int ed) { endOffset = ed; }
//
//	/** Returns this Token's lexical type.  Defaults to "word". */
//	string getType() { return type; }
//
//protected:
//	string termText;				  // the text of the term
//	int startOffset;				  // start in source text
//	int endOffset;				  // end in source text
//	string type;				  // lexical type
//};
