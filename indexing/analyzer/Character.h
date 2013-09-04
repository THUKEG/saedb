#pragma once
#include<string>
using std::string;
using std::wstring;

class Character
{
public:
    Character(void)
    {
    }
    ~Character(void)
    {
        //do nothing
    }
    /*
    * Variables
    */
    
    //static int MAX_CODE_POINT; //The maximum value of a Unicode code point.

    bool isLetter(char codePoint) {
        bool bLetter = false;
        bLetter = CharacterDataLatin1_isLetter(codePoint);
        return bLetter;
    }
     
    static char toLowerCase(char ch) {
        if(ch>='A' && ch<='Z')
            return (char)(ch-'A'+'a');
        else
            return ch;
    }

    static void toLowerCase(string& str) {
        int len = str.length();
        for(int i=0;i<len;i++){
            str[i] = toLowerCase(str[i]);
        }
    }
    /*
     * Utils for Character.isLetter...
     */
    bool CharacterDataLatin1_isLetter(int ch) {
        bool isLetter = false;
        if(ch>=97 && ch<=122) //A-Z
            isLetter = true;
        else if(ch>=65 && ch<=90) //a-z
            isLetter = true;
        else if(ch == 170 || ch == 181 || ch == 186)
            isLetter = true;
        else if( (ch>=192 && ch<=214) || (ch>=216 && ch<=246) || (ch>=248 && ch<=255)) 
            isLetter = true;
        return isLetter;
    }
    int getPlane(int ch) {
        return (ch >> 16);
    }
};

