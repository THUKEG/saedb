/***************************************************************************
                          english_stem.h  -  description
                             -------------------
    begin                : Sat May 25 2004
    copyright            : (C) 2004 by Blake Madden
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the BSD License.                                *
 *                                                                         *
 ***************************************************************************/

#ifndef __ENGLISH_STEM_H__
#define __ENGLISH_STEM_H__

#include "stemming.h"

using namespace common_lang_constants;

namespace stemming
    {
    /**Overview

    I have made more than one attempt to improve the structure of the Porter algorithm 
    by making it follow the pattern of ending removal of the Romance language stemmers.
    It is not hard to see why one should want to do this: step 1b of the Porter stemmer 
    removes ed and ing, which are i-suffixes (*) attached to verbs. If these suffixes are
    removed, there should be no need to remove d-suffixes which are not verbal, although it
    will try to do so. This seems to be a deficiency in the Porter stemmer, not shared by
    the Romance stemmers. Again, the divisions between steps 2, 3 and 4 seem rather arbitrary,
    and are not found in the Romance stemmers. 

    Nevertheless, these attempts at improvement have been abandoned. They seem to lead to a
    more complicated algorithm with no very obvious improvements. A reason for not taking
    note of the outcome of step 1b may be that English endings do not determine word categories
    quite as strongly as endings in the Romance languages. For example, condition and position
    in French have to be nouns, but in English they can be verbs as well as nouns,

    We are all conditioned by advertising
    They are positioning themselves differently today

    A possible reason for having separate steps 2, 3 and 4 is that d-suffix combinations in
    English are quite complex, a point which has been made elsewhere. 

    But it is hardly surprising that after twenty years of use of the Porter stemmer, certain
    improvements do suggest themselves, and a new algorithm for English is therefore offered
    here. (It could be called the 'Porter2' stemmer to distinguish it from the Porter stemmer,
    from which it derives.) The changes are not so very extensive: (1) terminating y is changed
    to i rather less often, (2) suffix us does not lose its s, (3) a few additional suffixes
    are included for removal, including (4) suffix ly. In addition, a small list of exceptional
    forms is included. In December 2001 there were two further adjustments: (5) Steps 5a and 5b
    of the old Porter stemmer were combined into a single step. This means that undoubling final
    ll is not done with removal of final e. (6) In Step 3 ative is removed only when in region R2. 

	To begin with, here is the basic algorithm without reference to the exceptional forms.
    An exact comparison with the Porter algorithm needs to be done quite carefully if done at
    all. Here we indicate by * points of departure, and by + additional features.
    In the sample vocabulary, Porter and Porter2 stem slightly under 5% of words to different forms.

    Dr. Martin Porter

    Define a vowel as one of 
        -a e i o u y 

    Define a double as one of 
        -bb dd ff gg mm nn pp rr tt 

    Define a valid li-ending as one of 
        -c d e g h k m n r t 

    Define a short syllable in a word as either (a) a vowel followed by a non-vowel
    other than w, x or Y and preceded by a non-vowel, or * (b) a vowel at the beginning
    of the word followed by a non-vowel. 

    So rap, trap, entrap end with a short syllable, and ow, on, at are classed as short syllables.
    But uproot, bestow, disturb do not end with a short syllable. 

    A word is called short if it consists of a short syllable preceded by zero or more consonants.
    R1 is the region after the first non-vowel following a vowel, or the end of the word if there is no such non-vowel.
    R2 is the region after the first non-vowel following a vowel in R1, or the end of the word if there is no such non-vowel.
    If the word has two letters or less, leave it as it is.
    Otherwise, do each of the following operations,
    Set initial y, or y after a vowel, to Y, and then establish the regions R1 and R2.*/

    //------------------------------------------------------
    template <typename string_typeT = std::wstring>
    class english_stem : public stem<string_typeT>
        {
    public:
        english_stem() : m_first_vowel(string_typeT::npos)
            {}
        //---------------------------------------------
        ///@param text: string to stem
        void operator()(string_typeT& text)
            {
            if (text.length() < 3)
                {
                return;
                }

            //reset internal data
            m_first_vowel = string_typeT::npos;
            stem<string_typeT>::reset_r_values();

            this->trim_western_punctuation(text);

            //handle exceptions first
            if (is_exception(text) )
                {
                return;
                }

            this->hash_y(text, L"aeiouyAEIOUY");
            m_first_vowel = text.find_first_of(L"aeiouyAEIOUY");
            if (m_first_vowel == string_typeT::npos)
                { return; }

            if (text.length() >= 5 &&
                /*gener*/
                (is_either<wchar_t>(text[0], LOWER_G, UPPER_G) &&
                    is_either<wchar_t>(text[1], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[2], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[3], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[4], LOWER_R, UPPER_R) ) )
                {
                stem<string_typeT>::set_r1(5);
                }
            else if (text.length() >= 6 &&
                /*commun*/
                (is_either<wchar_t>(text[0], LOWER_C, UPPER_C) &&
                    is_either<wchar_t>(text[1], LOWER_O, UPPER_O) &&
                    is_either<wchar_t>(text[2], LOWER_M, UPPER_M) &&
                    is_either<wchar_t>(text[3], LOWER_M, UPPER_M) &&
                    is_either<wchar_t>(text[4], LOWER_U, UPPER_U) &&
                    is_either<wchar_t>(text[5], LOWER_N, UPPER_N) ) )
                {
                stem<string_typeT>::set_r1(6);
                }
			else if (text.length() >= 5 &&
                /*arsen*/
                (is_either<wchar_t>(text[0], LOWER_A, UPPER_A) &&
                    is_either<wchar_t>(text[1], LOWER_R, UPPER_R) &&
                    is_either<wchar_t>(text[2], LOWER_S, UPPER_S) &&
                    is_either<wchar_t>(text[3], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[4], LOWER_N, UPPER_N) ) )
                {
                stem<string_typeT>::set_r1(5);
                }
            else
                {
                this->find_r1(text, L"aeiouyAEIOUY");
                }

            this->find_r2(text, L"aeiouyAEIOUY");

            //step 1a:
            step_1a(text);
            //exception #2
            if (is_exception_post_step1a(text) )
                {
                return;
                }
            //step 1b:
            step_1b(text);
            //step 1c:
            step_1c(text);
            //step 2:
            step_2(text);
            //step 3:
            step_3(text);
            //step 4:
            step_4(text);
            //step 5:
            step_5(text);

            this->unhash_y(text);
            }
    private:
        //---------------------------------------------
        bool is_exception(string_typeT& text)
            {
            //exception #0
            /*skis*/
            if (text.length() == 4 &&
                is_either<wchar_t>(text[0], LOWER_S, UPPER_S) &&
                is_either<wchar_t>(text[1], LOWER_K, UPPER_K) &&
                is_either<wchar_t>(text[2], LOWER_I, UPPER_I) &&
                is_either<wchar_t>(text[3], LOWER_S, UPPER_S) )
                {
                text = L"ski";
                return true;
                }
            /*skies*/
            else if (text.length() == 5 &&
                    is_either<wchar_t>(text[0], LOWER_S, UPPER_S) &&
                    is_either<wchar_t>(text[1], LOWER_K, UPPER_K) &&
                    is_either<wchar_t>(text[2], LOWER_I, UPPER_I) &&
                    is_either<wchar_t>(text[3], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[4], LOWER_S, UPPER_S) )
                {
                text = L"sky";
                return true;
                }
            /*dying*/
            else if (text.length() == 5 &&
                    is_either<wchar_t>(text[0], LOWER_D, UPPER_D) &&
                    is_either<wchar_t>(text[1], LOWER_Y, UPPER_Y) &&
                    is_either<wchar_t>(text[2], LOWER_I, UPPER_I) &&
                    is_either<wchar_t>(text[3], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[4], LOWER_G, UPPER_G) )
                {
                text = L"die";
                return true;
                }
            /*lying*/
            else if (text.length() == 5 &&
                    is_either<wchar_t>(text[0], LOWER_L, UPPER_L) &&
                    is_either<wchar_t>(text[1], LOWER_Y, UPPER_Y) &&
                    is_either<wchar_t>(text[2], LOWER_I, UPPER_I) &&
                    is_either<wchar_t>(text[3], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[4], LOWER_G, UPPER_G) )
                {
                text = L"lie";
                return true;
                }
            /*tying*/
            else if (text.length() == 5 &&
                    is_either<wchar_t>(text[0], LOWER_T, UPPER_T) &&
                    is_either<wchar_t>(text[1], LOWER_Y, UPPER_Y) &&
                    is_either<wchar_t>(text[2], LOWER_I, UPPER_I) &&
                    is_either<wchar_t>(text[3], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[4], LOWER_G, UPPER_G) )
                {
                text = L"tie";
                return true;
                }
            /*idly*/
            else if (text.length() == 4 &&
                    is_either<wchar_t>(text[0], LOWER_I, UPPER_I) &&
                    is_either<wchar_t>(text[1], LOWER_D, UPPER_D) &&
                    is_either<wchar_t>(text[2], LOWER_L, UPPER_L) &&
                    is_either<wchar_t>(text[3], LOWER_Y, UPPER_Y) )
                {
                text = L"idl";
                return true;
                }
            /*gently*/
            else if (text.length() == 6 &&
                    is_either<wchar_t>(text[0], LOWER_G, UPPER_G) &&
                    is_either<wchar_t>(text[1], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[2], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[3], LOWER_T, UPPER_T) &&
                    is_either<wchar_t>(text[4], LOWER_L, UPPER_L) &&
                    is_either<wchar_t>(text[5], LOWER_Y, UPPER_Y) )
                {
                text = L"gentl";
                return true;
                }
            /*ugly*/
            else if (text.length() == 4 &&
                    is_either<wchar_t>(text[0], LOWER_U, UPPER_U) &&
                    is_either<wchar_t>(text[1], LOWER_G, UPPER_G) &&
                    is_either<wchar_t>(text[2], LOWER_L, UPPER_L) &&
                    is_either<wchar_t>(text[3], LOWER_Y, UPPER_Y) )
                {
                text = L"ugli";
                return true;
                }
            /*early*/
            else if (text.length() == 5 &&
                    is_either<wchar_t>(text[0], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[1], LOWER_A, UPPER_A) &&
                    is_either<wchar_t>(text[2], LOWER_R, UPPER_R) &&
                    is_either<wchar_t>(text[3], LOWER_L, UPPER_L) &&
                    is_either<wchar_t>(text[4], LOWER_Y, UPPER_Y) )
                {
                text = L"earli";
                return true;
                }
            /*only*/
            else if (text.length() == 4 &&
                    is_either<wchar_t>(text[0], LOWER_O, UPPER_O) &&
                    is_either<wchar_t>(text[1], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[2], LOWER_L, UPPER_L) &&
                    is_either<wchar_t>(text[3], LOWER_Y, UPPER_Y) )
                {
                text = L"onli";
                return true;
                }
            /*singly*/
            else if (text.length() == 6 &&
                    is_either<wchar_t>(text[0], LOWER_S, UPPER_S) &&
                    is_either<wchar_t>(text[1], LOWER_I, UPPER_I) &&
                    is_either<wchar_t>(text[2], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[3], LOWER_G, UPPER_G) &&
                    is_either<wchar_t>(text[4], LOWER_L, UPPER_L) &&
                    is_either<wchar_t>(text[5], LOWER_Y, UPPER_Y) )
                {
                text = L"singl";
                return true;
                }
            //exception #1
            else if (
                /*sky*/
                (text.length() == 3 &&
                    is_either<wchar_t>(text[0], LOWER_S, UPPER_S) &&
                    is_either<wchar_t>(text[1], LOWER_K, UPPER_K) &&
                    is_either<wchar_t>(text[2], LOWER_Y, UPPER_Y) ) ||
                /*news*/
                (text.length() == 4 &&
                    is_either<wchar_t>(text[0], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[1], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[2], LOWER_W, UPPER_W) &&
                    is_either<wchar_t>(text[3], LOWER_S, UPPER_S) ) ||
                /*howe*/
                (text.length() == 4 &&
                    is_either<wchar_t>(text[0], LOWER_H, UPPER_H) &&
                    is_either<wchar_t>(text[1], LOWER_O, UPPER_O) &&
                    is_either<wchar_t>(text[2], LOWER_W, UPPER_W) &&
                    is_either<wchar_t>(text[3], LOWER_E, UPPER_E) ) ||
                /*atlas*/
                (text.length() == 5 &&
                    is_either<wchar_t>(text[0], LOWER_A, UPPER_A) &&
                    is_either<wchar_t>(text[1], LOWER_T, UPPER_T) &&
                    is_either<wchar_t>(text[2], LOWER_L, UPPER_L) &&
                    is_either<wchar_t>(text[3], LOWER_A, UPPER_A) &&
                    is_either<wchar_t>(text[4], LOWER_S, UPPER_S) ) ||
                /*cosmos*/
                (text.length() == 6 &&
                    is_either<wchar_t>(text[0], LOWER_C, UPPER_C) &&
                    is_either<wchar_t>(text[1], LOWER_O, UPPER_O) &&
                    is_either<wchar_t>(text[2], LOWER_S, UPPER_S) &&
                    is_either<wchar_t>(text[3], LOWER_M, UPPER_M) &&
                    is_either<wchar_t>(text[4], LOWER_O, UPPER_O) &&
                    is_either<wchar_t>(text[5], LOWER_S, UPPER_S) ) ||
                /*bias*/
                (text.length() == 4 &&
                    is_either<wchar_t>(text[0], LOWER_B, UPPER_B) &&
                    is_either<wchar_t>(text[1], LOWER_I, UPPER_I) &&
                    is_either<wchar_t>(text[2], LOWER_A, UPPER_A) &&
                    is_either<wchar_t>(text[3], LOWER_S, UPPER_S) ) ||
                /*andes*/
                (text.length() == 5 &&
                    is_either<wchar_t>(text[0], LOWER_A, UPPER_A) &&
                    is_either<wchar_t>(text[1], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[2], LOWER_D, UPPER_D) &&
                    is_either<wchar_t>(text[3], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[4], LOWER_S, UPPER_S) ) )
                {
                return true;
                }
            return false;
            }

        //---------------------------------------------
        bool is_exception_post_step1a(string_typeT& text)
            {
            //exception #2
            if (/*inning*/
                (text.length() == 6 &&
                    is_either<wchar_t>(text[0], LOWER_I, UPPER_I) &&
                    is_either<wchar_t>(text[1], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[2], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[3], LOWER_I, UPPER_I) &&
                    is_either<wchar_t>(text[4], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[5], LOWER_G, UPPER_G) ) ||
                /*outing*/
                (text.length() == 6 &&
                    is_either<wchar_t>(text[0], LOWER_O, UPPER_O) &&
                    is_either<wchar_t>(text[1], LOWER_U, UPPER_U) &&
                    is_either<wchar_t>(text[2], LOWER_T, UPPER_T) &&
                    is_either<wchar_t>(text[3], LOWER_I, UPPER_I) &&
                    is_either<wchar_t>(text[4], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[5], LOWER_G, UPPER_G) ) ||
                /*canning*/
                (text.length() == 7 &&
                    is_either<wchar_t>(text[0], LOWER_C, UPPER_C) &&
                    is_either<wchar_t>(text[1], LOWER_A, UPPER_A) &&
                    is_either<wchar_t>(text[2], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[3], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[4], LOWER_I, UPPER_I) &&
                    is_either<wchar_t>(text[5], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[6], LOWER_G, UPPER_G) ) ||
                /*herring*/
                (text.length() == 7 &&
                    is_either<wchar_t>(text[0], LOWER_H, UPPER_H) &&
                    is_either<wchar_t>(text[1], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[2], LOWER_R, UPPER_R) &&
                    is_either<wchar_t>(text[3], LOWER_R, UPPER_R) &&
                    is_either<wchar_t>(text[4], LOWER_I, UPPER_I) &&
                    is_either<wchar_t>(text[5], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[6], LOWER_G, UPPER_G) ) ||
                /*earring*/
                (text.length() == 7 &&
                    is_either<wchar_t>(text[0], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[1], LOWER_A, UPPER_A) &&
                    is_either<wchar_t>(text[2], LOWER_R, UPPER_R) &&
                    is_either<wchar_t>(text[3], LOWER_R, UPPER_R) &&
                    is_either<wchar_t>(text[4], LOWER_I, UPPER_I) &&
                    is_either<wchar_t>(text[5], LOWER_N, UPPER_N) &&
                    is_either<wchar_t>(text[6], LOWER_G, UPPER_G) ) ||
                /*proceed*/
                (text.length() == 7 &&
                    is_either<wchar_t>(text[0], LOWER_P, UPPER_P) &&
                    is_either<wchar_t>(text[1], LOWER_R, UPPER_R) &&
                    is_either<wchar_t>(text[2], LOWER_O, UPPER_O) &&
                    is_either<wchar_t>(text[3], LOWER_C, UPPER_C) &&
                    is_either<wchar_t>(text[4], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[5], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[6], LOWER_D, UPPER_D) ) ||
                /*exceed*/
                (text.length() == 6 &&
                    is_either<wchar_t>(text[0], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[1], LOWER_X, UPPER_X) &&
                    is_either<wchar_t>(text[2], LOWER_C, UPPER_C) &&
                    is_either<wchar_t>(text[3], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[4], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[5], LOWER_D, UPPER_D) ) ||
                /*succeed*/
                (text.length() == 7 &&
                    is_either<wchar_t>(text[0], LOWER_S, UPPER_S) &&
                    is_either<wchar_t>(text[1], LOWER_U, UPPER_U) &&
                    is_either<wchar_t>(text[2], LOWER_C, UPPER_C) &&
                    is_either<wchar_t>(text[3], LOWER_C, UPPER_C) &&
                    is_either<wchar_t>(text[4], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[5], LOWER_E, UPPER_E) &&
                    is_either<wchar_t>(text[6], LOWER_D, UPPER_D) ) )
                {
                return true;
                }
            return false;
            }
        /**Search for the longest among the following suffixes, and perform the action indicated. 

            -sses 
                -replace by ss 

            -ied+   ies* 
                -replace by i if preceded by just one letter, otherwise by ie (so ties -> tie, cries -> cri) 

            -s 
                -delete if the preceding word part contains a vowel not immediately before the s (so gas and this retain the s, gaps and kiwis lose it) 

            -us+   ss 
                -do nothing*/
        //---------------------------------------------
        void step_1a(string_typeT& text)
            {
            if (this->is_suffix(text,/*sses*/LOWER_S, UPPER_S, LOWER_S, UPPER_S, LOWER_E, UPPER_E, LOWER_S, UPPER_S) )
                {
                text.erase(text.length()-2);
                this->update_r_sections(text);
                }
            else if (this->is_suffix(text,/*ied*/LOWER_I, UPPER_I, LOWER_E, UPPER_E, LOWER_D, UPPER_D) ||
                    this->is_suffix(text,/*ies*/LOWER_I, UPPER_I, LOWER_E, UPPER_E, LOWER_S, UPPER_S) )
                {
                if (text.length() == 3 || text.length() == 4)
                    {
                    text.erase(text.length()-1);
                    this->update_r_sections(text);
                    }
                else
                    {
                    text.erase(text.length()-2);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 2 &&
                    is_either<wchar_t>(text[text.length()-1], LOWER_S, UPPER_S) &&
                    m_first_vowel < text.length()-2 &&
                    !string_util::is_one_of(text[text.length()-2], L"suSU") )
                {
                text.erase(text.length()-1);
                this->update_r_sections(text);
                }
            }
        /**Search for the longest among the following suffixes, and perform the action indicated. 
        
            -eed   eedly+ 
                -replace by ee if in R1 

            -ed   edly+   ing   ingly+ 
                -delete if the preceding word part contains a vowel, and then 
                -if the word ends at, bl or iz add e (so luxuriat -> luxuriate), or 
                -if the word ends with a double remove the last letter (so hopp -> hop), or 
                -if the word is short, add e (so hop -> hope)*/
        //---------------------------------------------
        void step_1b(string_typeT& text)
            {
            //if the preceding word contains a vowel
            bool regress_trim = false;

            if (this->is_suffix(text,/*eed*/LOWER_E, UPPER_E, LOWER_E, UPPER_E, LOWER_D, UPPER_D) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-3)
                    {
                    text.erase(text.length()-1);
                    this->update_r_sections(text);
                    }
                }
            else if (this->is_suffix(text,/*eedly*/LOWER_E, UPPER_E, LOWER_E, UPPER_E, LOWER_D, UPPER_D, LOWER_L, UPPER_L, LOWER_Y, UPPER_Y) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-5)
                    {
                    text.erase(text.length()-3);
                    this->update_r_sections(text);
                    }
                }
            else if (this->is_suffix(text,/*ed*/LOWER_E, UPPER_E, LOWER_D, UPPER_D) &&
                m_first_vowel < text.length()-2)
                {
                text.erase(text.length()-2);
                this->update_r_sections(text);
                regress_trim = true;
                }
            else if (this->is_suffix(text,/*edly*/LOWER_E, UPPER_E, LOWER_D, UPPER_D, LOWER_L, UPPER_L, LOWER_Y, UPPER_Y) &&
                m_first_vowel < text.length()-4)
                {
                text.erase(text.length()-4);
                this->update_r_sections(text);
                regress_trim = true;
                }
            else if (this->is_suffix(text,/*ing*/LOWER_I, UPPER_I, LOWER_N, UPPER_N, LOWER_G, UPPER_G) &&
                m_first_vowel < text.length()-3)
                {
                text.erase(text.length()-3);
                this->update_r_sections(text);
                regress_trim = true;
                }
            else if (this->is_suffix(text,/*ingly*/LOWER_I, UPPER_I, LOWER_N, UPPER_N, LOWER_G, UPPER_G, LOWER_L, UPPER_L, LOWER_Y, UPPER_Y) &&
                m_first_vowel < text.length()-5)
                {
                text.erase(text.length()-5);
                this->update_r_sections(text);
                regress_trim = true;
                }
            if (regress_trim)
                {
                if (this->is_suffix(text,/*at*/LOWER_A, UPPER_A, LOWER_T, UPPER_T) ||
                    this->is_suffix(text,/*bl*/LOWER_B, UPPER_B, LOWER_L, UPPER_L) ||
                    this->is_suffix(text,/*iz*/LOWER_I, UPPER_I, LOWER_Z, UPPER_Z) )
                    {
                    text += LOWER_E;
                    //need to search for r2 again because the 'e' added here may change that
                    this->find_r2(text, L"aeiouyAEIOUY");
                    }
                else if (this->is_suffix(text,/*bb*/LOWER_B, UPPER_B, LOWER_B, UPPER_B) ||
                        this->is_suffix(text,/*dd*/LOWER_D, UPPER_D, LOWER_D, UPPER_D) ||
                        this->is_suffix(text,/*ff*/LOWER_F, UPPER_F, LOWER_F, UPPER_F) ||
                        this->is_suffix(text,/*gg*/LOWER_G, UPPER_G, LOWER_G, UPPER_G) ||
                        this->is_suffix(text,/*mm*/LOWER_M, UPPER_M, LOWER_M, UPPER_M) ||
                        this->is_suffix(text,/*nn*/LOWER_N, UPPER_N, LOWER_N, UPPER_N) ||
                        this->is_suffix(text,/*pp*/LOWER_P, UPPER_P, LOWER_P, UPPER_P) ||
                        this->is_suffix(text,/*rr*/LOWER_R, UPPER_R, LOWER_R, UPPER_R) ||
                        this->is_suffix(text,/*tt*/LOWER_T, UPPER_T, LOWER_T, UPPER_T) )
                    {
                    text.erase(text.length()-1);
                    this->update_r_sections(text);
                    }
                else if (is_short_word(text, text.length() ) )
                    {
                    text += LOWER_E;
                    //need to search for r2 again because the 'e' added here may change that
                    this->find_r2(text, L"aeiouyAEIOUY");
                    }
                }
            }
        /**replace suffix y or Y by i if preceeded by a non-vowel which is
        not the first letter of the word (so cry -> cri, by -> by, say -> say)*/
        //---------------------------------------------
        void step_1c(string_typeT& text)
            {
            //proceeding consonant cannot be first letter in word
            if (text.length() > 2 &&
                !is_vowel(text[text.length()-2]) )
                {
                if (is_either<wchar_t>(text[text.length()-1], LOWER_Y, LOWER_Y_HASH) )
                    {
                    text[text.length()-1] = LOWER_I;
                    }
                else if (is_either<wchar_t>(text[text.length()-1], UPPER_Y, UPPER_Y_HASH) )
                    {
                    text[text.length()-1] = UPPER_I;
                    }
                }
            }
        /**Search for the longest among the following suffixes, and, if found and in R1,
        perform the action indicated. 

            -tional:   replace by tion 
            -enci:   replace by ence 
            -anci:   replace by ance 
            -abli:   replace by able 
            -entli:   replace by ent 
            -izer   ization:   replace by ize 
            -ational   ation   ator:   replace by ate 
            -alism   aliti   alli:   replace by al 
            -fulness:   replace by ful 
            -ousli   ousness:   replace by ous 
            -iveness   iviti:   replace by ive 
            -biliti   bli+:   replace by ble 
            -ogi+:   replace by og if preceded by l 
            -fulli+:   replace by ful 
            -lessli+:   replace by less 
            -li+:   delete if preceded by a valid li-ending*/
        //---------------------------------------------
        void step_2(string_typeT& text)
            {
            if (text.length() >= 7 &&
				(this->is_suffix(text,/*ization*/LOWER_I, UPPER_I, LOWER_Z, UPPER_Z, LOWER_A, UPPER_A, LOWER_T, UPPER_T, LOWER_I, UPPER_I, LOWER_O, UPPER_O, LOWER_N, UPPER_N) ||
				this->is_suffix(text,/*ational*/LOWER_A, UPPER_A, LOWER_T, UPPER_T, LOWER_I, UPPER_I, LOWER_O, UPPER_O, LOWER_N, UPPER_N, LOWER_A, UPPER_A, LOWER_L, UPPER_L) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-7)
                    {
                    text.erase(text.length()-4);
                    text[static_cast<int>(text.length()-1)] = LOWER_E;
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 7 &&
				(this->is_suffix(text,/*fulness*/LOWER_F, UPPER_F, LOWER_U, UPPER_U, LOWER_L, UPPER_L, LOWER_N, UPPER_N, LOWER_E, UPPER_E, LOWER_S, UPPER_S, LOWER_S, UPPER_S) ||
				this->is_suffix(text,/*ousness*/LOWER_O, UPPER_O, LOWER_U, UPPER_U, LOWER_S, UPPER_S, LOWER_N, UPPER_N, LOWER_E, UPPER_E, LOWER_S, UPPER_S, LOWER_S, UPPER_S) ||
				this->is_suffix(text,/*iveness*/LOWER_I, UPPER_I, LOWER_V, UPPER_V, LOWER_E, UPPER_E, LOWER_N, UPPER_N, LOWER_E, UPPER_E, LOWER_S, UPPER_S, LOWER_S, UPPER_S) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-7)
                    {
                    text.erase(text.length()-4);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 6 &&
				(this->is_suffix(text,/*tional*/LOWER_T, UPPER_T, LOWER_I, UPPER_I, LOWER_O, UPPER_O, LOWER_N, UPPER_N, LOWER_A, UPPER_A, LOWER_L, UPPER_L) ||
				this->is_suffix(text,/*lessli*/LOWER_L, UPPER_L, LOWER_E, UPPER_E, LOWER_S, UPPER_S, LOWER_S, UPPER_S, LOWER_L, UPPER_L, LOWER_I, UPPER_I) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-6)
                    {
                    text.erase(text.length()-2);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 6 && this->is_suffix(text,/*biliti*/LOWER_B, UPPER_B, LOWER_I, UPPER_I, LOWER_L, UPPER_L, LOWER_I, UPPER_I, LOWER_T, UPPER_T, LOWER_I, UPPER_I) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-6)
                    {
                    text.erase(text.length()-3);
                    text[text.length()-2] = LOWER_L;
                    text[text.length()-1] = LOWER_E;
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 5 &&
				(this->is_suffix(text,/*iviti*/LOWER_I, UPPER_I, LOWER_V, UPPER_V, LOWER_I, UPPER_I, LOWER_T, UPPER_T, LOWER_I, UPPER_I) ||
				this->is_suffix(text,/*ation*/LOWER_A, UPPER_A, LOWER_T, UPPER_T, LOWER_I, UPPER_I, LOWER_O, UPPER_O, LOWER_N, UPPER_N) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-5)
                    {
                    text.erase(text.length()-2);
                    text[text.length()-1] = LOWER_E;
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 5 &&
				(this->is_suffix(text,/*alism*/LOWER_A, UPPER_A, LOWER_L, UPPER_L, LOWER_I, UPPER_I, LOWER_S, UPPER_S, LOWER_M, UPPER_M) ||
				this->is_suffix(text,/*aliti*/LOWER_A, UPPER_A, LOWER_L, UPPER_L, LOWER_I, UPPER_I, LOWER_T, UPPER_T, LOWER_I, UPPER_I) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-5)
                    {
                    text.erase(text.length()-3);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 5 &&
				(this->is_suffix(text,/*ousli*/LOWER_O, UPPER_O, LOWER_U, UPPER_U, LOWER_S, UPPER_S, LOWER_L, UPPER_L, LOWER_I, UPPER_I) ||
				this->is_suffix(text,/*entli*/LOWER_E, UPPER_E, LOWER_N, UPPER_N, LOWER_T, UPPER_T, LOWER_L, UPPER_L, LOWER_I, UPPER_I) ||
				this->is_suffix(text,/*fulli*/LOWER_F, UPPER_F, LOWER_U, UPPER_U, LOWER_L, UPPER_L, LOWER_L, UPPER_L, LOWER_I, UPPER_I) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-5)
                    {
                    text.erase(text.length()-2);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 4 && this->is_suffix(text,/*alli*/LOWER_A, UPPER_A, LOWER_L, UPPER_L, LOWER_L, UPPER_L, LOWER_I, UPPER_I) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-4)
                    {
                    text.erase(text.length()-2);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 4 &&
				(this->is_suffix(text,/*enci*/LOWER_E, UPPER_E, LOWER_N, UPPER_N, LOWER_C, UPPER_C, LOWER_I, UPPER_I) ||
				this->is_suffix(text,/*anci*/LOWER_A, UPPER_A, LOWER_N, UPPER_N, LOWER_C, UPPER_C, LOWER_I, UPPER_I) ||
				this->is_suffix(text,/*abli*/LOWER_A, UPPER_A, LOWER_B, UPPER_B, LOWER_L, UPPER_L, LOWER_I, UPPER_I) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-4)
                    {
                    text[text.length()-1] = LOWER_E;
                    }
                }
            else if (text.length() >= 4 && this->is_suffix(text,/*izer*/LOWER_I, UPPER_I, LOWER_Z, UPPER_Z, LOWER_E, UPPER_E, LOWER_R, UPPER_R) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-4)
                    {
                    text.erase(text.length()-1);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 4 && this->is_suffix(text,/*ator*/LOWER_A, UPPER_A, LOWER_T, UPPER_T, LOWER_O, UPPER_O, LOWER_R, UPPER_R) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-4)
                    {
                    text.erase(text.length()-1);
                    text[text.length()-1] = LOWER_E;
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 3 &&
                stem<string_typeT>::get_r1() <= (text.length()-3) &&
                this->is_suffix(text,/*bli*/LOWER_B, UPPER_B, LOWER_L, UPPER_L, LOWER_I, UPPER_I) )
                {
                text[text.length()-1] = LOWER_E;
                }
            else if (text.length() >= 3 &&
                stem<string_typeT>::get_r1() <= (text.length()-3) &&
                this->is_suffix(text,/*ogi*/LOWER_O, UPPER_O, LOWER_G, UPPER_G, LOWER_I, UPPER_I) )
                {
                if (is_either<wchar_t>(text[text.length()-4], LOWER_L, UPPER_L) )
                    {
                    text.erase(text.length()-1);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 2 &&
                    stem<string_typeT>::get_r1() <= (text.length()-2) &&
                    this->is_suffix(text,/*li*/LOWER_L, UPPER_L, LOWER_I, UPPER_I) )
                {
                if (string_util::is_one_of(text[text.length()-3], L"cdeghkmnrtCDEGHKMNRT") )
                    {
                    text.erase(text.length()-2);
                    this->update_r_sections(text);
                    }
                }
            }
        /**Search for the longest among the following suffixes, and, if found and in R1, perform the action indicated. 

            -tional+:   replace by tion 
            -ational+:   replace by ate 
            -alize:   replace by al 
            -icate   iciti   ical:   replace by ic 
            -ful   ness:   delete 
            -ative*:   delete if in R2*/
        //---------------------------------------------
        void step_3(string_typeT& text) 
            {
            if (text.length() >= 7 && this->is_suffix(text,/*ational*/LOWER_A, UPPER_A, LOWER_T, UPPER_T, LOWER_I, UPPER_I, LOWER_O, UPPER_O, LOWER_N, UPPER_N, LOWER_A, UPPER_A, LOWER_L, UPPER_L) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-7)
                    {
                    text.erase(text.length()-4);
                    text[text.length()-1] = LOWER_E;
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 6 && this->is_suffix(text,/*tional*/LOWER_T, UPPER_T, LOWER_I, UPPER_I, LOWER_O, UPPER_O, LOWER_N, UPPER_N, LOWER_A, UPPER_A, LOWER_L, UPPER_L) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-6)
                    {
                    text.erase(text.length()-2);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 5 &&
				(this->is_suffix(text,/*icate*/LOWER_I, UPPER_I, LOWER_C, UPPER_C, LOWER_A, UPPER_A, LOWER_T, UPPER_T, LOWER_E, UPPER_E) ||
				this->is_suffix(text,/*iciti*/LOWER_I, UPPER_I, LOWER_C, UPPER_C, LOWER_I, UPPER_I, LOWER_T, UPPER_T, LOWER_I, UPPER_I) ||
				this->is_suffix(text,/*alize*/LOWER_A, UPPER_A, LOWER_L, UPPER_L, LOWER_I, UPPER_I, LOWER_Z, UPPER_Z, LOWER_E, UPPER_E) ) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-5)
                    {
                    text.erase(text.length()-3);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 5 && this->is_suffix(text,/*ative*/LOWER_A, UPPER_A, LOWER_T, UPPER_T, LOWER_I, UPPER_I, LOWER_V, UPPER_V, LOWER_E, UPPER_E) )
                {
                if (stem<string_typeT>::get_r2() <= text.length()-5)
                    {
                    text.erase(text.length()-5);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 4 && this->is_suffix(text,/*ical*/LOWER_I, UPPER_I, LOWER_C, UPPER_C, LOWER_A, UPPER_A, LOWER_L, UPPER_L) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-4)
                    {
                    text.erase(text.length()-2);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 4 && this->is_suffix(text,/*ness*/LOWER_N, UPPER_N, LOWER_E, UPPER_E, LOWER_S, UPPER_S, LOWER_S, UPPER_S) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-4)
                    {
                    text.erase(text.length()-4);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 3 && this->is_suffix(text,/*ful*/LOWER_F, UPPER_F, LOWER_U, UPPER_U, LOWER_L, UPPER_L) )
                {
                if (stem<string_typeT>::get_r1() <= text.length()-3)
                    {
                    text.erase(text.length()-3);
                    this->update_r_sections(text);
                    }
                }
            }
        /**Search for the longest among the following suffixes, and, if found and in R2, perform the action indicated. 

            -al ance ence er ic able ible ant ement ment ent ism ate iti ous ive ize 
                -delete 
            -ion 
                -delete if preceded by s or t*/
        //---------------------------------------------
        void step_4(string_typeT& text)
            {
            if (text.length() >= 5 &&
                this->is_suffix(text,/*ement*/LOWER_E, UPPER_E, LOWER_M, UPPER_M, LOWER_E, UPPER_E, LOWER_N, UPPER_N, LOWER_T, UPPER_T) )
                {
                if (stem<string_typeT>::get_r2() <= text.length()-5)
                    {
                    text.erase(text.length()-5);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 4 &&
                    (this->is_suffix(text,/*able*/LOWER_A, UPPER_A, LOWER_B, UPPER_B, LOWER_L, UPPER_L, LOWER_E, UPPER_E) ||
                    this->is_suffix(text,/*ible*/LOWER_I, UPPER_I, LOWER_B, UPPER_B, LOWER_L, UPPER_L, LOWER_E, UPPER_E) ||
                    this->is_suffix(text,/*ment*/LOWER_M, UPPER_M, LOWER_E, UPPER_E, LOWER_N, UPPER_N, LOWER_T, UPPER_T) ||
                    this->is_suffix(text,/*ence*/LOWER_E, UPPER_E, LOWER_N, UPPER_N, LOWER_C, UPPER_C, LOWER_E, UPPER_E) ||
                    this->is_suffix(text,/*ance*/LOWER_A, UPPER_A, LOWER_N, UPPER_N, LOWER_C, UPPER_C, LOWER_E, UPPER_E)) )
                {
                if (stem<string_typeT>::get_r2() <= text.length()-4)
                    {
                    text.erase(text.length()-4);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 4 &&
                    (this->is_suffix(text,/*sion*/LOWER_S, UPPER_S, LOWER_I, UPPER_I, LOWER_O, UPPER_O, LOWER_N, UPPER_N) ||
                    this->is_suffix(text,/*tion*/LOWER_T, UPPER_T, LOWER_I, UPPER_I, LOWER_O, UPPER_O, LOWER_N, UPPER_N)) )
                {
                if (stem<string_typeT>::get_r2() <= text.length()-3)
                    {
                    text.erase(text.length()-3);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 3 &&
                    (this->is_suffix(text,/*ant*/LOWER_A, UPPER_A, LOWER_N, UPPER_N, LOWER_T, UPPER_T) ||
                    this->is_suffix(text,/*ent*/LOWER_E, UPPER_E, LOWER_N, UPPER_N, LOWER_T, UPPER_T) ||
                    this->is_suffix(text,/*ism*/LOWER_I, UPPER_I, LOWER_S, UPPER_S, LOWER_M, UPPER_M) ||
                    this->is_suffix(text,/*ate*/LOWER_A, UPPER_A, LOWER_T, UPPER_T, LOWER_E, UPPER_E) ||
                    this->is_suffix(text,/*iti*/LOWER_I, UPPER_I, LOWER_T, UPPER_T, LOWER_I, UPPER_I) ||
                    this->is_suffix(text,/*ous*/LOWER_O, UPPER_O, LOWER_U, UPPER_U, LOWER_S, UPPER_S) ||
                    this->is_suffix(text,/*ive*/LOWER_I, UPPER_I, LOWER_V, UPPER_V, LOWER_E, UPPER_E) ||
                    this->is_suffix(text,/*ize*/LOWER_I, UPPER_I, LOWER_Z, UPPER_Z, LOWER_E, UPPER_E)) )
                {
                if (stem<string_typeT>::get_r2() <= text.length()-3)
                    {
                    text.erase(text.length()-3);
                    this->update_r_sections(text);
                    }
                }
            else if (text.length() >= 2 &&
                    (this->is_suffix(text,/*al*/LOWER_A, UPPER_A, LOWER_L, UPPER_L) ||
                    this->is_suffix(text,/*er*/LOWER_E, UPPER_E, LOWER_R, UPPER_R) ||
                    this->is_suffix(text,/*ic*/LOWER_I, UPPER_I, LOWER_C, UPPER_C)) )
                {
                if (stem<string_typeT>::get_r2() <= text.length()-2)
                    {
                    text.erase(text.length()-2);
                    this->update_r_sections(text);
                    }
                }
            }
        /**Search for the the following suffixes, and, if found, perform the action indicated. 

            -e 
                -delete if in R2, or in R1 and not preceded by a short syllable 
            -l 
                -delete if in R2 and preceded by l*/
        //---------------------------------------------
        void step_5(string_typeT& text)
            {
            if (text.length() >= 1 &&
                is_either<wchar_t>(text[text.length()-1], LOWER_E, UPPER_E) )
                {
                if (stem<string_typeT>::get_r2() != text.length())
                    {
                    text.erase(text.length()-1);
                    this->update_r_sections(text);
                    }
                else if (stem<string_typeT>::get_r1() != text.length() &&
                    text.length() >= 2 &&
                    //look at the part of the word in front of the last 'e' to see if it ends with
                    //a short syllable.
                    !ends_with_short_syllable(text, text.length()-1))
                    {
                    text.erase(text.length()-1);
                    this->update_r_sections(text);
                    }
                }
            else if (stem<string_typeT>::get_r2() != text.length() &&
                this->is_suffix(text,/*ll*/LOWER_L, UPPER_L, LOWER_L, UPPER_L) )
                {
                text.erase(text.length()-1);
                this->update_r_sections(text);
                }
            }
        /**Define a short syllable in a word as either
        (a) a vowel followed by a non-vowel other than w, x or Y and preceded by a non-vowel, or 
        (b) a vowel at the beginning of the word followed by a non-vowel.

        So rap, trap, entrap end with a short syllable, and ow, on, at are classed as short syllables.
        But uproot, bestow, disturb do not end with a short syllable.*/
        //---------------------------------------------
        bool ends_with_short_syllable(string_typeT& text, const size_t length) const
            {
            if (length == 2)
                {
                if (is_vowel(text[0]) )
                    {
                    return (!is_vowel(text[1]) );
                    }
                else
                    {
                    return false;
                    }
                }
            else if (length > 2)
                {
                size_t start = text.find_last_of(L"aeiouyAEIOUY", length-1);
                if (start == string_typeT::npos)
                    {
                    return false;
                    }
                if (start > 0 &&
                    start == (length-2) &&
                    //following letter
                    (!is_vowel(text[start+1]) &&
                    !string_util::is_one_of(text[start+1], L"wxWX") &&
                    is_neither(text[start+1], LOWER_Y_HASH, UPPER_Y_HASH)) &&
                    //proceeding letter
                    !is_vowel(text[start-1]) )
                    {
                    return true;
                    }
                else
                    {
                    return false;
                    }
                }
            else
                {
                return false;
                }
            }
		///A word is called short if it ends in a short syllable, and if R1 is null.
        //---------------------------------------------
        inline bool is_short_word(string_typeT& text, const size_t length) const
            {
            return (ends_with_short_syllable(text, length) && stem<string_typeT>::get_r1() == text.length());
            }
        //---------------------------------------------
        inline bool is_vowel(wchar_t character) const
            {
            return (string_util::is_one_of(character, L"aeiouyAEIOUY") );
            }

        size_t m_first_vowel;
        };
    }

#endif //__ENGLISH_STEM_H__
