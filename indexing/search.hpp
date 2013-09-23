#pragma once
#include <string>
#include <vector>
#include "indexing.hpp"
#include "query.hpp"

namespace indexing {

struct SearchResult : public std::vector<QueryItem> {
};

struct Searcher {
    Searcher(const Index& index) : index(index) {
    }

    SearchResult search(TokenStream* stream) {
        SearchResult result;
        auto q = buildQuery(stream, index);
        if (q) {
            QueryItem item;
            while (q->next(item)) {
                result.push_back(item);
            }
        }
        return result;
    }

private:
    const Index& index;
};

}
