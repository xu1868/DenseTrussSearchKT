#include "TrussDecomposition.hpp"
#include "Utility.hpp"
#include <algorithm>

using namespace std;

void TrussDecomposition::initSupport() {
    for (auto beg = graph->BegEI(); beg != graph->EndEI(); beg++) {
        int support = computeSupport(graph, beg);
        auto nodePair = minmax_element(beg.GetSrcNId(), beg.GetDstNId());
        edgeSupport[nodePair] = support;
        sortedEdgeSupport.insert({support, nodePair});
    }
}

void TrussDecomposition::initTrussness() {
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
        // The first condition is to prevent it from entering an endless loop...
        while (!sortedSupport.empty() &&
               sortedSupport.begin()->first <= k - 2) {
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
    // For nodes not in any edge
    for (auto beg = graph->BegNI(); beg != graph->EndNI(); beg++) {
        int nodeId = beg.GetId();
        if (!nodeTrussness.count(nodeId)) {
            nodeTrussness[nodeId] = 2;
        }
    }

    maxTrussness = k - 1;
}

void TrussDecomposition::initTrussSubgraph() {
    // init `trussSubgraph`.
    for (int i = 0; i <= maxTrussness; ++i) {
        PUndirNet subgraph = TUndirNet::New();
        trussSubgraph.push_back(subgraph);
    }

    for (auto beg = graph->BegEI(); beg != graph->EndEI(); beg++) {
        EId edge = minmax_element(beg.GetSrcNId(), beg.GetDstNId());
        int trussness = edgeTrussness[edge];
        auto [u, v] = edge;
        for (int i = trussness; i >= 0; --i) {
            if (!trussSubgraph[i]->IsNode(u)) {
                trussSubgraph[i]->AddNode(u);
            }
            if (!trussSubgraph[i]->IsNode(v)) {
                trussSubgraph[i]->AddNode(v);
            }
            if (!trussSubgraph[i]->IsEdge(u, v)) {
                trussSubgraph[i]->AddEdge(u, v);
            }
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