#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <memory>
#include "analyzer.hpp"

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
    std::vector<short> positions;
    double score;

    bool operator<(const indexing::PostingItem& x) const
    {
        return docId < x.docId;
    }

};

struct PostingList : public std::set<PostingItem> {
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
    int id(const std::string word);
    int findId(const std::string word) const;
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
