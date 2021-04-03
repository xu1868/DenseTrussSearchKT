#include "DenseTrussSearchTopDown.hpp"
#include <algorithm>
#include <numeric>
#include <utility>

using namespace std;

DenseTrussSearchTopDown::~DenseTrussSearchTopDown() { delete trussDecom; }

void DenseTrussSearchTopDown::topdown() {
    /// Equivalent to \tau' in alg 3
    map<int, int> keywordMaxTruss;

    // Calculate the max truss for each keyword
    for (int i = 0; i < keywords.size(); ++i) {
        for (auto node : keywords[i]) {
            if (keywordMaxTruss.find(i) == keywordMaxTruss.end()) {
                keywordMaxTruss[i] = nodeTrussness.at(node);
            } else {
                keywordMaxTruss[i] =
                    max(keywordMaxTruss[i], nodeTrussness.at(node));
            }
        }
    }

    // Choose k_max
    int kMax = INT32_MAX;
    for (auto trussness : keywordMaxTruss) {
        kMax = min(kMax, trussness.second);
    }

    /// This is the k_max-truss chosen for each k_max value.
    PUndirNet kMaxTruss = TUndirNet::New();

    while (result.Empty()) {
        for (auto beg = graph->BegEI(); beg != graph->EndEI(); beg++) {
            auto [u, v] = make_pair(beg.GetSrcNId(), beg.GetDstNId());
            if (edgeTrussness.at({u, v}) >= kMax) {
                if (!kMaxTruss->IsNode(u)) {
                    kMaxTruss->AddNode(u);
                }
                if (!kMaxTruss->IsNode(v)) {
                    kMaxTruss->AddNode(v);
                }
                if (!kMaxTruss->IsEdge(u, v)) {
                    kMaxTruss->AddEdge(u, v);
                }
            }
        }

        TCnComV cntrussSubgraph;
        TSnap::GetWccs(graph, cntrussSubgraph);
        for (int i = 0; i < cntrussSubgraph.Len(); ++i) {
            TCnCom cnCom = cntrussSubgraph[i];
            vector<int> computed(keywords.size(), 0);
            int tag = 0;
            for (int j = 0; j < cnCom.Len(); ++j) {
                int node = cnCom[j];
                for (int k = 0; k < keywords.size(); ++k) {
                    if (find(keywords[k].begin(), keywords[k].end(), node) !=
                        keywords[k].end()) {
                        computed[k] = 0;
                    }
                }
                tag = accumulate(computed.begin(), computed.end(), 0);
                if (tag == keywords.size()) {
                    if (result.Empty() || result.Len() >= cnCom.Len()) {
                        result = cnCom;
                    }
                }
            }
        }
        kMax--;
    }
}