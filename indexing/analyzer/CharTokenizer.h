#pragma once
#include<iostream>
#include "Tokenizer.h"
#include "TokenUtil.h"

using std::string;

class CharTokenizer :
    public Tokenizer
{
public:
    CharTokenizer(void)
        :    ioBuffer(NULL)
    {
        init();
    }

    ~CharTokenizer(void)
    {
        //delete[] ioBuffer;
    }
    //string input;
    CharTokenizer(string in)
        :    Tokenizer(in),
             ioBuffer(NULL)
    {
        init();
    }

    virtual bool isTokenChar(char c)=0; //TODO

    bool next(Token& token)
    {
        int length = 0;
        int start = offset;
        const char* cur_ptr = ioBuffer+offset, *end_ptr = ioBuffer+dataLen;
        while (cur_ptr!=end_ptr)
        {
            ++offset;
            if (isTokenChar(*cur_ptr))
            {
                ++length;
            }
            else
            {
                if (length) break;
                start = offset;
            }
            ++cur_ptr;
        }

        if (length)
        {
            //wstring wstr(ioBuffer+start, ioBuffer+start+length);
            //token.setTermText(TokenUtil::ws2s(wstr));
            token.setTermText(ioBuffer,start,length);
            token.setStartOffset(start);
            token.setEndOffset(start+length);
            return true;
        }
        else
        {
            return false;
        }
    }
    
    void reset(){
        offset = 0;
    }; 

private:
    int offset, dataLen;
    const char* ioBuffer;

    void init()
    {
        offset = 0;
        dataLen = input.length();
        ioBuffer = input.c_str();
    }
};

