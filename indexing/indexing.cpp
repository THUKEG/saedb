#include <string>
#include <ctype.h>
#include "indexing.hpp"

using namespace std;
using namespace indexing;

int WordMap::id(const std::string word) {
    auto wi = find(word);
    if (wi != end()) {
        return wi->second;
    } else {
        int i = (int) size();
        insert(make_pair(word, i));
        return i;
    }
}

int WordMap::findId(const std::string word) const {
    auto wi = find(word);
    if (wi != end()) {
        return wi->second;
    }
    else return -1;
}

void Index::addSingle(int doc, int field, TokenStream* stream, double avg_len) {
    //unique_ptr<TokenStream> stream(ArnetAnalyzer::tokenStream(value));
    unordered_map<int, vector<short>> word_position;
    int position = 0;
    Token token;
    while (stream->next(token)) {
        string term = token.getTermText();
        int term_id = word_map.id(term);
        word_position[term_id].push_back(position++);
    }

    int totalTokens = position;
    for (auto& wp : word_position) {
        int word = wp.first;
        auto& positions = wp.second;
        // insert a new posting item
        Term term{word, field};
        double score = bm25(positions.size(), totalTokens, avg_len);
        (*this)[term].insert(PostingItem{doc, positions, score});
    }
}

Index Index::build(DocumentCollection docs) {
    Index index;
    int count = docs.size();
    double avgLen = 0;
    for (auto& doc : docs) {
        for (auto& field : doc.second) {
            string value = field.value;
            for (unsigned i = 0; i < value.length(); i++)
                if (value[i] == ' ' && i != value.length()-1)
                    avgLen++;
            avgLen++;
        }
    }
    avgLen = avgLen / (double)count;

    for (auto& doc : docs) {
        for (auto& field : doc.second) {
            unique_ptr<TokenStream> stream(ArnetAnalyzer::tokenStream(field.value));
            index.addSingle(doc.second.id, 0, stream.get(), avgLen);
        }
    }

    index.optimize();
    return std::move(index);
}

void Index::optimize() {
    // currently nothing to do.
}

