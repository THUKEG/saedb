#pragma once
#include <unordered_map>
#include "TokenFilter.h"
#include "./StemmerLib/english_stem.h"
#include "TokenUtil.h"

using std::unordered_map;

class StemFilter :
    public TokenFilter
{
public:
    StemFilter(TokenStream* in);
    bool next(Token& token);
private:
    stemming::english_stem<> StemEnglish;
};

