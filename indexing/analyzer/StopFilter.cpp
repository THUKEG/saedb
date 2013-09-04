#include "StopFilter.h"

unordered_set<string> StopFilter::stopTokens = {
	"a", "an", "and", "are", "as", "at", "be", "but", "by", "for",
	"if", "in", "into", "is", "it", "no", "not", "of", "on", "or",
	"such", "that", "the", "their", "then", "there", "these", "they", "this",
	"to", "was", "will", "with"
};

StopFilter::StopFilter(TokenStream* in) : TokenFilter(in) { }

bool StopFilter::next(Token& token) {
	bool more = input->next(token);
	while (more)
	{
		const string& str = token.getWsTermText();
		if (stopTokens.find(str) == stopTokens.end())
		{
			return true;
		}
		more = input->next(token);
	}
	return false;
}
