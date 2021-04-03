#include "KTIndex.hpp"

using namespace std;

void KTIndex::buildKTIndex() {
    for (int i = 0; i < trussSubgraph.size(); ++i) {
        TCnComV CCs = trussCC[i];

        /// To be inserted into `nodesInCC`
        vector<vector<int>> nodeNumbers;

        for (int j = 0; j < CCs.Len(); ++j) {
            TCnCom cc = CCs[j];
            cc.Sort();
            vector<int> ccNodes;
            for (int k = 0; k < cc.Len(); ++k) {
                ccNodes.push_back(cc[k]);
            }
            vector<int> nodesInCCsContainingKw;
            for (int k = 0; k < keywords.size(); ++k) {
                vector<int> kwNodes = keywords[k];
                sort(kwNodes.begin(), kwNodes.end());
                set_intersection(kwNodes.begin(), kwNodes.end(),
                                 ccNodes.begin(), ccNodes.end(),
                                 back_inserter(nodesInCCsContainingKw));
                if (!nodesInCCsContainingKw.empty()) {
                    keywordIndex[k][i].push_back(j);
                    keywordMaxTrussN[k] = i;
                }
                sort(nodesInCCsContainingKw.begin(),
                     nodesInCCsContainingKw
                         .end()); // I have regretted giving such a long name...
                nodesInCCsContainingKw.erase(
                    unique(nodesInCCsContainingKw.begin(),
                           nodesInCCsContainingKw.end()),
                    nodesInCCsContainingKw.end());
            }
            nodeNumbers.push_back(nodesInCCsContainingKw);
        }
        nodesInCC.push_back(nodeNumbers);
    }
}
