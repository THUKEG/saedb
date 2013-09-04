#include "StemFilter.h"
#include "TokenUtil.h"

StemFilter::StemFilter(TokenStream* in)
	:TokenFilter(in)
{

}

bool StemFilter::next(Token& token){
	bool more = input->next(token);
	if (!more) return false;

	string str = token.getWsTermText();
	string originalStr = str;
	wstring temp = TokenUtil::s2ws(str);
	StemEnglish(temp);
        string res = TokenUtil::ws2s(temp);
	token.setTermText(res);
	return true;
}
