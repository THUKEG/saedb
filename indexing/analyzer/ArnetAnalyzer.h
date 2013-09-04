#pragma once
#include "TokenStream.h"
#include "LowerCaseFilter.h"
#include "LetterTokenizer.h"
#include "StemFilter.h"
#include "StopFilter.h"

class ArnetAnalyzer
{
public:

    ArnetAnalyzer(void)
    {
    }

    ~ArnetAnalyzer(void)
    {
    }

    static TokenStream* tokenStream(const string& input)
    {
        TokenStream* le = new LetterTokenizer(input);
        le = new LowerCaseFilter(le);
        le = new StemFilter(le);
        le = new StopFilter(le);
        return le;
    }
};

