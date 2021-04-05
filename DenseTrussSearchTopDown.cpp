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
        // construct subgraph for kMax-truss
        for (auto beg = graph->BegEI(); beg != graph->EndEI(); beg++) {
            auto [u, v] = make_pair(beg.GetSrcNId(), beg.GetDstNId());
            if (edgeTrussness.at(minmax_element(u, v)) >= kMax) {
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
        TSnap::GetWccs(kMaxTruss, cntrussSubgraph);
        for (int i = 0; i < cntrussSubgraph.Len(); ++i) {
            TCnCom cnCom = cntrussSubgraph[i];
            set<int> nodes;
            bool ok = true;
            for (int i = 0; i < cnCom.Len(); ++i) {
                nodes.insert(cnCom[i]);
            }
            for (int i = 0; i < keywords.size(); ++i) {
                vector<int> tmp;
                set_intersection(nodes.begin(), nodes.end(),
                                 keywords[i].begin(), keywords[i].end(),
                                 back_inserter(tmp));
                if (tmp.empty()) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                if (result.Empty() || result.Len() > cnCom.Len()) {
                    result = cnCom;
                }
            }
        }
        if (--kMax < 2) {
            break;
        }
    }

    endTrussness = kMax + 1;
}