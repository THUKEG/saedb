#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <memory>
#include "analyzer.hpp"
#include "serialization/serialization.hpp"

namespace indexing {

struct Term {
    int word;
    int field;

    inline bool operator==(Term const &another) const
    {
        return (word == another.word) && (field == another.field);
    }
};

}

namespace std {

template <>
struct hash<indexing::Term> {
    size_t operator()(const indexing::Term term) const {
        return std::hash<int>()(term.word) ^ std::hash<int>()(term.field);
    }
};

}

namespace indexing {

const float BM25_K = 2.0;
const float BM25_B = 0.75;

struct PostingItem {
    int docId;
    double score;
    std::vector<short> positions;

    PostingItem() = default;

    PostingItem(int docId, double score, std::vector<short>&& positions)
        : docId(docId), score(score), positions(positions) {
    }

    bool operator<(const indexing::PostingItem& x) const
    {
        return docId < x.docId;
    }

};

struct PostingList : public std::vector<PostingItem> {
};

struct Field {
    std::string name;
    std::string value;
};

struct Document : public std::vector<Field> {
    int id;
};

struct DocumentCollection : public std::map<int, Document> {
};

struct WordMap : public std::unordered_map<std::string, int> {
    int id(const std::string& word);
    int findId(const std::string& word) const;
};

struct Index : public std::unordered_map<Term, PostingList> {
    WordMap word_map;

    // add single field
    void addSingle(int doc, int field, TokenStream* stream, double score);

    // optimize the index
    void optimize();

    static Index build(DocumentCollection);
};

inline double bm25(int freq, int total_tokens, double avg_len) {
    return (freq * (BM25_K + 1)) / (freq + BM25_K * (1 - BM25_B + BM25_B * total_tokens / avg_len));
}

} // namespace indexing

namespace sae{namespace serialization{ namespace custom_serialization_impl{

    template<>
    struct serialize_impl<OSerializeStream, indexing::Term>{
        static void run(OSerializeStream& ostr, const indexing::Term& a){
                ostr << a.word << a.field;
        }
    };

    template <>
    struct deserialize_impl<ISerializeStream, indexing::Term>{
            static void run(ISerializeStream& istr, indexing::Term& a){
                    istr>> a.word >> a.field;
            }
    };

    template<>
    struct serialize_impl<OSerializeStream, indexing::PostingItem>{
        static void run(OSerializeStream& ostr, const indexing::PostingItem& p){
            ostr << p.docId << p.positions << p.score;
        }
    };

    template<>
    struct deserialize_impl<ISerializeStream, indexing::PostingItem>{
        static void run(ISerializeStream& istr, indexing::PostingItem& p){
            istr >> p.docId >> p.positions >> p.score;
        }
    };


    template<>
    struct serialize_impl<OSerializeStream,indexing::Index>{
        static void run(OSerializeStream& ostr, const indexing::Index& i){
            const std::unordered_map<indexing::Term, indexing::PostingList>* index = &i;
            const std::unordered_map<std::string, int>* wm = &(i.word_map);
            ostr << (*index) << (*wm);
        }
    };

    template<>
    struct deserialize_impl<ISerializeStream,indexing::Index>{
         static void run(ISerializeStream& istr, indexing::Index& i){
             std::unordered_map<indexing::Term, indexing::PostingList>* index = &i;
             std::unordered_map<std::string, int>* wm = &(i.word_map);
             istr >> (*index);
             istr >> (*wm);
        }
    };


    template<>
    struct serialize_impl<OSerializeStream,indexing::PostingList>{
        static void run(OSerializeStream& ostr, const indexing::PostingList& pl){
            const std::vector<indexing::PostingItem>* pp = &pl;
            ostr << (*pp);
        }
    };

    template<>
    struct deserialize_impl<ISerializeStream,indexing::PostingList>{
        static void run(ISerializeStream& istr,indexing::PostingList& pl){
            std::vector<indexing::PostingItem>* pp = &pl;
            istr >> (*pp);
        }
    };

}}}
