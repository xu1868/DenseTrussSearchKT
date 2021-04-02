#include "DenseTrussSearch.hpp"
#include <algorithm>
#include <numeric>

using namespace std;

void DenseTrussSearch::initSupport() {
    for (auto beg = graph->BegEI(); beg != graph->EndEI(); beg++) {
        int support = computeSupport(graph, beg);
        auto nodePair = minmax_element(beg.GetSrcNId(), beg.GetDstNId());
        edgeSupport[nodePair] = support;
        sortedEdgeSupport.insert({support, nodePair});
    }
}

void DenseTrussSearch::initTrussness() {
    /* Note: in this function I am going to **copy** supports calculated in
     * previous functions, which differs from the original implementation. This
     * is because supports might be used in further changes. Also, this function
     * should be seldom invoked (only once for each graph).
     */
    int k = 2;
    auto sortedSupport = sortedEdgeSupport;
    auto support = edgeSupport;

    /* Main idea: find out the edge with the least support. Remove the edge, and
     * decrease its neighbors' supports by 1.
     */
    while (!sortedSupport.empty()) {
        // While still exists edge e that sup(e) <= k - 2
        // Sort in ascending order
        while (sortedSupport.begin()->first <= k - 2) {
            EId edge = sortedSupport.begin()->second;
            int sup = sortedSupport.begin()->first;
            // ensure that deg(u) <= deg(v)
            auto [u, v] = minMaxDegVertices(graph, edge.first, edge.second);

            for (int i = 0; i < graph->GetNI(u).GetDeg(); ++i) {
                VId w = graph->GetNI(u).GetNbrNId(i);
                if (support.find(minmax_element(u, w)) != support.end() &&
                    support.find(minmax_element(w, v)) != support.end()) {
                    // sup((u,w)) -= 1
                    int uw_support = support[minmax_element(u, w)];
                    sortedSupport.erase(
                        make_pair(uw_support, minmax_element(u, w)));
                    support[minmax_element(u, w)] -= 1;
                    uw_support -= 1;
                    sortedSupport.insert(
                        make_pair(uw_support, minmax_element(u, w)));
                    // sup((v,w)) -= 1
                    int vw_support = support[minmax_element(v, w)];
                    sortedSupport.erase(
                        make_pair(vw_support, minmax_element(v, w)));
                    support[minmax_element(v, w)] -= 1;
                    vw_support -= 1;
                    sortedSupport.insert(
                        make_pair(vw_support, minmax_element(v, w)));
                }
            }
            edgeTrussness[edge] = k;
            nodeTrussness[edge.first] = k;
            nodeTrussness[edge.second] = k;
            support.erase(edge);
            sortedSupport.erase({sup, edge});
        }
        k += 1;
    }
    maxTrussness = k - 1;
}

void DenseTrussSearch::initTrussSubgraph() {
    // init `trussSubgraph`.
    for (int i = 0; i <= maxTrussness; ++i) {
        PUndirNet subgraph = TUndirNet::New();
        trussSubgraph.push_back(subgraph);
    }

    for (auto beg = graph->BegEI(); beg != graph->EndEI(); beg++) {
        EId edge = std::make_pair(beg.GetSrcNId(), beg.GetDstNId());
        int trussness = edgeTrussness[edge];
        if (!trussSubgraph[trussness]->IsNode(beg.GetSrcNId())) {
            trussSubgraph[trussness]->AddNode(beg.GetSrcNId());
        }
        if (!trussSubgraph[trussness]->IsNode(beg.GetDstNId())) {
            trussSubgraph[trussness]->AddNode(beg.GetDstNId());
        }
        if (!trussSubgraph[trussness]->IsEdge(beg.GetSrcNId(),
                                              beg.GetDstNId())) {
            trussSubgraph[trussness]->AddEdge(beg.GetSrcNId(), beg.GetDstNId());
        }
    }

    // init `trussCC`.
    for (int i = 0; i < trussSubgraph.size(); ++i) {
        PUndirNet subgraph = trussSubgraph[i];
        TCnComV CCs;
        TSnap::GetWccs(subgraph, CCs);
        trussCC.push_back(CCs);
    }
}

TCnCom DenseTrussSearch::topdown() {
    /// Equivalent to \tau' in alg 3
    map<int, int> keywordMaxTruss;

    // Calculate the max truss for each keyword
    for (int i = 0; i < keywords.size(); ++i) {
        for (auto node : keywords[i]) {
            if (!keywordMaxTruss.contains(i)) {
                keywordMaxTruss[i] = nodeTrussness[node];
            } else {
                keywordMaxTruss[i] =
                    max(keywordMaxTruss[i], nodeTrussness[node]);
            }
        }
    }

    // Choose k_max
    int kMax = INT32_MAX;
    for (auto trussness : keywordMaxTruss) {
        kMax = min(kMax, trussness.second);
    }

    TCnCom result;
    /// This is the k_max-truss chosen for each k_max value.
    PUndirNet kMaxTruss = TUndirNet::New();

    while (result.Empty()) {
        for (auto beg = graph->BegEI(); beg != graph->EndEI(); beg++) {
            auto [u, v] = make_pair(beg.GetSrcNId(), beg.GetDstNId());
            if (edgeTrussness[{u, v}] >= kMax) {
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

    return result;
}

std::tuple<std::map<int, int>, std::map<int, std::map<int, std::vector<int>>>,
           std::vector<std::vector<std::vector<int>>>>
DenseTrussSearch::buildKTIndex() {
    map<int, int> keywordMaxTrussN;
    map<int, map<int, vector<int>>> keywordIndex;
    vector<vector<vector<int>>> nodesInCC;

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

    return make_tuple(keywordMaxTrussN, keywordIndex, nodesInCC);
}
