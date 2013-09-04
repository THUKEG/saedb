#ifndef __PUNCTUATION_H__
#define __PUNCTUATION_H__

#include "utilities.h"
#include "common_lang_constants.h"

using namespace common_lang_constants;

namespace punctuation
    {
    /**ispunct for ISO 8891-1 and MS 1252.
    This is more locale-independent and also handles surrogates (e.g., the Euro
    symbol under 1252) that the regular ispunct would miss.*/
    inline bool is_western_punct(wchar_t ch)
        {
        //see if it is either a space, control character, or alphanumeric and negate that
        return !(((ch >= 0x30/*'0'*/) && (ch <= 0x39/*'9'*/)) ||
            ((ch >= 0x41/*'A'*/) && (ch <= 0x5A/*'Z'*/)) ||
            ((ch >= 0x61/*'a'*/) && (ch <= 0x7A/*'z'*/)) ||
            //uppercase extended ASCII set
            ((ch >= 0xC0) && (ch <= 0xD6)) ||
            ((ch >= 0xD8) && (ch <= 0xDF)) ||
            //lowercase extended ASCII set
            ((ch >= 0xE0) && (ch <= 0xF6)) ||
            ((ch >= 0xF8) && (ch <= 0xFF)) ||
            //spaces and control characters
            ((ch >= 0x00) && (ch <= 0x20)) );
        }

    //is_hypthen functors
    //---------------------------
    class is_western_hypthen
        {
    public:
        inline bool operator()(const wchar_t& ch) const
            {
            return ch == 45;
            }
        };

    class is_western_punctuation
        {
    public:
        inline bool operator()(const wchar_t& ch) const
            {
            return is_western_punct(ch);
            }
        };

    //is_character functors
    //---------------------------
    class is_western_character
        {
    public:
        inline static bool is_dash(const wchar_t& ch)
            {
            return (ch == 0x002D) ? //hypthen
                true : (ch == 0x2012) ? //hypthen
                true : (ch == 0x2012) ? //figure dash
                true : (ch == 0x2013) ? //en dash
                true : (ch == 0x2014) ? //em dash
                true : (ch == 0x2015) ? //horizontal bar
                true : false;
            }
        inline static bool is_apostrophe(const wchar_t& ch)
            {
            return (ch == 39) ? //'
                true : (ch == 146) ? //apostrophe
                true : (ch == 180) ? //apostrophe
                true : (ch == 0x2019) ? //right single apostrophe
                true : false;
            }
        inline bool can_character_begin_word(const wchar_t& ch) const
            {
            return (ch >= 35 && ch <= 36) ? /*#$*/
                true : (ch  == 38) ? /*&*/
                true : (ch >= 48 && ch <= 57) ? /*0-9*/
                true : (ch >= UPPER_A && ch <= UPPER_Z) ? /*A-Z*/
                true : (ch >= LOWER_A && ch <= LOWER_Z) ? /*a-z*/
                true : (ch == 0x9F) ? //Y with diaeresis
                true : (ch  == 163) ? //Pound Sterling
                true : (ch  == 177) ? //plus/minus?
                true : (ch >= 192 && ch <= 246) ?
                true : (ch >= 248 && ch <= 255) ?
                true : false;
            }
        inline bool can_character_begin_word_uppercase(const wchar_t& ch) const
            {
            return (ch >= 35 && ch <= 36) ? /*#$*/
                true : (ch  == 38) ? /*&*/
                true : (ch >= 48 && ch <= 57) ? /*0-9*/
                true : (ch >= UPPER_A && ch <= UPPER_Z) ? /*A-Z*/
                true : (ch == 0x9F) ? //Y with diaeresis
                true : (ch  == 163) ? //Pound Sterling
                true : (ch  == 177) ? //plus/minus
                true : (ch >= 192 && ch <= 223) ?
                true : false;
            }
        inline bool can_character_end_word(const wchar_t& ch) const
            {
            return (ch == 35) ? /*#*/
                true : (ch >= 37 && ch <= 38) ? /*%&*/
                true : (ch == 46) ? //.
                true : (ch >= 47 && ch <= 57) ? /*/0-9*/
                true : (ch >= UPPER_A && ch <= UPPER_Z) ? /*A-Z*/
                true : (ch == 92) ? /*\*/
                true : (ch >= LOWER_A && ch <= LOWER_Z) ? /*a-z*/
                true : (ch == 0x9F) ? //Y with diaeresis
                true : (ch == 162) ? //cent
                true : (ch == 176) ? //degree
                true : (ch >= 192 && ch <= 246) ?
                true : (ch >= 248 && ch <= 255) ?
                true : false;
            }
        inline bool can_character_prefix_numeral(const wchar_t& ch) const
            {
            return (ch == 35) ? //#
                true : (ch == 36) ? //$
                true : (ch >= 43 && ch <= 46) ? //+,-.
                true : is_either<wchar_t>(ch, 0x80, 0x20AC) ? //Euro
                true : (ch == 163) ? //Pound Sterling
                true : (ch == 165) ? //Yen
                true : (ch == 177) ? //plus/minus
                true : false;
            }
        inline bool is_numeric(const wchar_t& ch) const
            {
            return (ch >= 48 && ch <= 57) ? /*0-9*/
                true : false;
            }
        inline bool is_ellipsis(const wchar_t* text, const size_t length) const
            {
            if (text == NULL || text[0] == 0 || length == 0)
                { return false; }
            if (length == 1)
                { return is_either<wchar_t>(text[0], 0x85, 0x2026); }
            size_t periodCount = 0;
            for (size_t i = 0; i < length; ++i)
                {
                if (text[i] == 0)
                    { break; }
                if (text[i] == PERIOD)
                    { ++periodCount; }
                }
            return (periodCount > 1);
            }
        inline bool can_character_form_date_time(const wchar_t& ch) const
            {
            return (ch >= 44 && ch <= 47) ? /*,-./*/
                true : (ch == 58) ? /*:*/
                true : false;
            }
        inline bool can_character_form_monetary(const wchar_t& ch) const
            {
            return (ch == 44 || ch == 46) ? /*,.*/
                true : false;
            }
        inline bool is_quote(const wchar_t& ch) const
            {
            return (ch == 34) ? /*"*/
                true : (ch == 39) ? /*'*/
                true : (ch == 96) ? /*`*/
                true : (ch == 130 || ch == 0x201A) ? //curved single quote
                true : (ch == 132 || ch == 0x201E) ? //curved double quote
                true : (ch == 139 || ch == 0x2039) ? //left single quote
                true : (ch == 155 || ch == 0x203A) ? //right single quote
                true : (ch == 171 || ch == 187) ? //left/right double quote («»)
                true : (ch >= 145 && ch <= 148) ? /*Windows 1252 quote surrogates*/
                true : (ch >= 0x2018 && ch <= 0x201D) ? /*unicode quotes*/
                true : false;
            }
        //--------------------------------------------------
        inline bool is_vowel(const wchar_t& letter) const
            {
            return ( (letter == LOWER_A) ||
                    (letter == LOWER_E) ||
                    (letter == LOWER_I) ||
                    (letter == LOWER_O) ||
                    (letter == LOWER_U) ||
                    (letter == LOWER_Y) ||
                    (letter == UPPER_A) ||
                    (letter == UPPER_E) ||
                    (letter == UPPER_I) ||
                    (letter == UPPER_O) ||
                    (letter == UPPER_U) ||
                    (letter == UPPER_Y) ||
                    (letter >= 0xC0 && letter <= 0xC6) ||
                    (letter >= 0xC8 && letter <= 0xCF) ||
                    (letter >= 0xD2 && letter <= 0xD6) ||
                    (letter >= 0xD8 && letter <= 0xDC) ||
                    (letter >= 0xE0 && letter <= 0xE6) ||
                    (letter >= 0xE8 && letter <= 0xEF) ||
                    (letter >= 0xF2 && letter <= 0xF6) ||
                    (letter >= 0xF8 && letter <= 0xFC) );
            }
        //--------------------------------------------------
        inline bool is_lower_consanent(const wchar_t& letter) const
            {
            return ( (letter >= LOWER_B && letter <= LOWER_D) ||
                    (letter >= LOWER_F && letter <= LOWER_H) ||
                    (letter >= LOWER_J && letter <= LOWER_N) ||
                    (letter >= LOWER_P && letter <= LOWER_T) ||
                    (letter >= LOWER_V && letter <= LOWER_X) || //just treat 'y' as a vowel for the sake of argument
                    (letter == LOWER_Z) ||
                    (letter == ESZETT) ||
                    (letter == LOWER_C_CEDILLA) ||
                    (letter == LOWER_N_TILDE) ||
                    (letter >= LOWER_Y_ACUTE && letter <= Y_UMLAUT));
            }
        //--------------------------------------------------
        inline bool is_consanent(const wchar_t& letter) const
            {
            return ( (letter >= UPPER_B && letter <= UPPER_D) ||
                    (letter >= UPPER_F && letter <= UPPER_H) ||
                    (letter >= UPPER_J && letter <= UPPER_N) ||
                    (letter >= UPPER_P && letter <= UPPER_T) ||
                    (letter >= UPPER_V && letter <= UPPER_X) || //just treat 'y' as a vowel for the sake of argument
                    (letter == UPPER_Z) ||
                    (letter >= LOWER_B && letter <= LOWER_D) ||
                    (letter >= LOWER_F && letter <= LOWER_H) ||
                    (letter >= LOWER_J && letter <= LOWER_N) ||
                    (letter >= LOWER_P && letter <= LOWER_T) ||
                    (letter >= LOWER_V && letter <= LOWER_X) || //just treat 'y' as a vowel for the sake of argument
                    (letter == LOWER_Z) ||
                    (letter == UPPER_C_CEDILLA) ||
                    (letter == UPPER_N_TILDE) ||
                    (letter >= UPPER_Y_ACUTE && letter <= ESZETT) ||
                    (letter == LOWER_C_CEDILLA) ||
                    (letter == LOWER_N_TILDE) ||
                    (letter >= LOWER_Y_ACUTE && letter <= Y_UMLAUT));
            }
        inline bool operator()(const wchar_t& ch) const
            {
            return (ch == 35) ? /*#*/
                true : (ch >= 37 && ch <= 39) ? /*%&'*/
                true : (ch >= 45 && ch <= 58) ? /*-./0-9:*/
                true : (ch >= 64 && ch <= 90) ? /*@, a-z*/
                true : (ch == 92) ? /*\*/
                true : (ch >= 95 && ch <= 122) ? /*_`A-Z*/
                true : (ch == 146) ? //apostrophe
                true : (ch == 159) ? //Y with diaeresis
                true : (ch == 162) ? //cent
                true : (ch == 176) ? //degree
                true : (ch == 180) ? //apostrophe
                true : (ch >= 192 && ch <= 246) ?
                true : (ch >= 248 && ch <= 255) ?
                true : (ch == 0x2019) ? //right single apostrophe
                true : false;
            }
        };

    ///is_space functor
    class is_western_space
        {
    public:
        inline bool operator()(const wchar_t& ch) const
            {
            return (ch == 32) ?
                true : (ch == 13) ?
                true : (ch == 10) ?
                true : (ch == 9) ?
                true : (ch == 160/*no break space*/) ?                
                true : false;
            }
        };

    // bullet or text indentation functors
    //---------------------------
    class is_western_bullet_or_intented_text
        {
    public:
        inline bool operator()(const wchar_t* text) const
            {
            if (text == NULL || text[0] == 0)
                { return false; }
            //first see if it's a tab or bullet
            if (text[0] == TAB ||
                is_either<wchar_t>(text[0], 0x95, 0x2022)/*bullet*/ ||
                text[0] == 0xB7/*middle dot*/)
                { return true; }
            //or a dash followed by a space
            if (isHypthen(text[0]) && text[1] && text[1] == SPACE)
                { return true; }
            //else, see if it more then two spaces (more than likely and indent)
            const wchar_t* current_char = text;
            while (current_char)
                {
                if (current_char[0] == SPACE)
                    { ++current_char; }
                else
                    { break; }
                }
            if ((current_char - text) > 2)
                { return true; }
            //or a numeric bullet
            if (text[0] >= 48 && text[0] <= 57/*0-9*/)
                {
                //scan until we hit something non-numeric
                do
                    {
                    if (text[0] >= 48 && text[0] <= 57/*0-9*/)
                        { ++text; }
                    else
                        { break; }
                    } while (text[0]);
                //if at the end of the text then this is not a bullet
                if (text[0] == 0)
                    { return false; }
                //if number if followed by a dot then it is a numeric bullet
                else if (text[0] == PERIOD ||
                    text[0] == TAB ||
                    text[0] == RIGHT_PARENTHESIS ||
                    text[0] == COLON)
                    { return true; }
                //anything else means that this probably is not a numeric bullet
                else
                    { return false; }
                }
            return false;
            }
    private:
        is_western_hypthen isHypthen;
        };

    //is_end_of_line functors
    //---------------------------
    class is_western_end_of_line
        {
    public:
        is_western_end_of_line() :
          m_characters_skipped_count(0),
          m_eol_count(0)
            {}
        inline bool operator()(const wchar_t& ch) const
            {
            return (ch == 13 || ch == 10);
            }
        //counts how many unique end of line characters (or sets) are in a ch stream
        inline void operator()(const wchar_t* first, const wchar_t* last)
            {
            //reset data from last call
            m_characters_skipped_count = 0;
            m_eol_count = 0;

            size_t lineFeedCount = 0, carriageReturnCount = 0;
            while (first != last)
                {
                if (first[0] == 10)
                    {
                    ++lineFeedCount;
                    }
                else if (first[0] == 13)
                    {
                    ++carriageReturnCount;
                    }
                else
                    {
                    break;
                    }
                ++first;
                ++m_characters_skipped_count;
                }
            //if no line feeds, this only carriage returns where used
            if (lineFeedCount == 0)
                {
                m_eol_count = carriageReturnCount;
                }
            //if no carriage returns, this only line feeds where used
            else if (carriageReturnCount == 0)
                {
                m_eol_count = lineFeedCount;
                }
            /*sometime 10 and 13 are used as a single eol separator,
            so pair them together if necessary*/
            else
                {
                //if they were perfectly paired up, then count them as sets
                if (carriageReturnCount == lineFeedCount)
                    {
                    m_eol_count = lineFeedCount;
                    }
                /*otherwise, the text is malformed.  just pair them off and then
                add the extra characters to it.*/
                else if (carriageReturnCount > lineFeedCount)
                    {
                    m_eol_count = lineFeedCount + (carriageReturnCount - lineFeedCount);
                    }
                else if (lineFeedCount > carriageReturnCount)
                    {
                    m_eol_count = carriageReturnCount + (lineFeedCount - carriageReturnCount);
                    }
                else
                    {
                    m_eol_count = 0;
                    }
                }
            }
        inline size_t get_characters_skipped_count() const
            { return m_characters_skipped_count; }
        inline size_t get_eol_count() const
            { return m_eol_count; }
    private:
        size_t m_characters_skipped_count;
        size_t m_eol_count;
        };

    /*Western punctuation counting functor utility*/
    class western_punctuation_count
        {
    public:
        inline size_t operator()(const wchar_t* text, const size_t length) const
            {
            /*special case where if the word is just '&' then treat
            it like a regular character (e.g., not punctuation)*/
            if (length == 1 && text[0] == common_lang_constants::AMPERSAND)
                { return 0; }
            return std::count_if(text, text+length, is_western_punct);
            }
        };

    ///use in tokenizer to keep track of punctuation marks in between words
    class punctuation_mark
        {
    public:
        punctuation_mark(const wchar_t mark,
                         const size_t word_position,
                         const bool isConnectedToPreviousWord)  :
            m_mark(mark), m_word_position(word_position),
            m_is_connected_to_previous_word(isConnectedToPreviousWord)
            {}
        inline size_t get_word_position() const
            { return m_word_position; }
        inline wchar_t get_punctuation_mark() const
            { return m_mark; }
        inline bool is_connected_to_previous_word() const
            { return m_is_connected_to_previous_word; }
    private:
        ///mark is in front of this word
        wchar_t m_mark;
        size_t m_word_position;
        bool m_is_connected_to_previous_word;
        };
    }

#endif //__PUNCTUATION_H__
