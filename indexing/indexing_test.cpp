#include <algorithm>
#include <cmath>
#include <fstream>

#include "testing/testharness.hpp"
#include "serialization/serialization.hpp"
#include "indexing.hpp"
#include "search.hpp"

using namespace indexing;
using namespace std;

struct SearchTest {
    SearchTest() {
        Document doc1, doc2, doc3;

        doc1.id = 1;
        doc2.id = 2;
        doc3.id = 3;
        doc1.push_back({"title", "initial document"});
        doc1.push_back({"content", "To maximize the scale of the community around a project, by reducing the friction for new Contributors and creating a scaled participation model with strong positive feedbacks;"});
        doc2.push_back({"title", "second document"});
        doc2.push_back({"content", "To relieve dependencies on key individuals by separating different skill sets so that there is a larger pool of competence in any required domain;"});
        doc3.push_back({"title", "third document"});
        doc3.push_back({"content", "To allow the project to develop faster and more accurately, by increasing the diversity of the decision making process;"});

        dc[1] = doc1;
        dc[2] = doc2;
        dc[3] = doc3;
    }

protected:
    DocumentCollection dc;
};

TEST(SearchTest, IndexAndSearch) {
    // This test will first build index, then save it,
    // then load it and use it for search.

    {
        // build index
        Index index = Index::build(dc);

        // save index
        ofstream fout("input.bin",std::fstream::binary);
        sae::serialization::OSerializeStream ostr(&fout);
        ostr << index;
        fout.close();
    }

    Index index2;
    {
        std::ifstream fin("input.bin",std::fstream::binary);
        sae::serialization::ISerializeStream istr(&fin);
        istr >> index2;
        fin.close();
    }

    Searcher searcher(index2);
    string query = "project develop";
    std::unique_ptr<TokenStream> stream (ArnetAnalyzer::tokenStream(query));
    SearchResult result = searcher.search(stream.get());
    sort(result.begin(), result.end());
    ASSERT_TRUE(result.size() == 2);
    ASSERT_TRUE(result[0].docId == 3);
    ASSERT_TRUE(abs(result[0].score - 17.561) < 1);
    ASSERT_TRUE(result[1].docId == 1);
    ASSERT_TRUE(abs(result[1].score - 1.3) < 1);
}

int main() {
    return ::saedb::test::RunAllTests();
}
