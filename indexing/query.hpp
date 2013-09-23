#pragma once
#include <string>
#include <memory>
#include <vector>
#include "indexing.hpp"
#include "analyzer.hpp"

namespace indexing {

struct QueryItem {
    int docId;
    double score;
};

inline bool operator< (const QueryItem& left, const QueryItem& right) {
    return left.score > right.score;
}

struct Query {
    virtual ~Query() = default;
    virtual bool next(QueryItem&) = 0;
};

struct AndQuery : public Query {
    AndQuery(std::unique_ptr<Query> leftOp, std::unique_ptr<Query> rightOp, double leftWeight = 1.0, double rightWeight = 1.0);
    virtual bool next(QueryItem& item);

    std::unique_ptr<Query> left, right;
    double leftFactor, rightFactor;
};

struct OrQuery : public Query {

    OrQuery(std::unique_ptr<Query> leftOp, std::unique_ptr<Query> rightOp, double leftWeight = 1.0, double rightWeight = 1.0);
    virtual bool next(QueryItem& item);

    std::unique_ptr<Query> left, right;
    double leftFactor, rightFactor;

private:
    bool hasLeft = false, hasRight = false;
    QueryItem u, v;
};

struct TermQuery : public Query {
    TermQuery(const Index& index, Term &term, int occur);
    virtual bool next(QueryItem& item);

private:
    PostingList::const_iterator it, end;
    int occurence;
};

std::unique_ptr<Query> TryCreateTermQuery(const std::string& term, const Index& index);


// Query Analyzers

struct QueryAnalyzer {
    virtual ~QueryAnalyzer() {}
    virtual std::unique_ptr<Query> buildQuery(TokenStream*, const Index&) = 0;
};

struct StandardQueryAnalyzer : public QueryAnalyzer
{
    std::unique_ptr<Query> buildQuery(TokenStream* stream, const Index& index);

protected:
    std::unique_ptr<Query> BuildOrQueryTree(std::vector<std::unique_ptr<Query>>& queries, int start, int end);
    std::unique_ptr<Query> BuildAndQueryTree(std::vector<std::unique_ptr<Query>>& queries, int start, int end);
    std::unique_ptr<Query> MergeWithOrQuery(std::unique_ptr<Query>& left, std::unique_ptr<Query>& right);
    std::unique_ptr<Query> MergeWithAndQuery(std::unique_ptr<Query>& left, std::unique_ptr<Query>& right);
};

std::unique_ptr<Query> buildQuery(TokenStream* stream, const Index& index);

} // namespace indexing
