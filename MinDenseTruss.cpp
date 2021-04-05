#include "MinDenseTruss.hpp"
#include "BanksAlgorithm.hpp"
#include "TrussDecomposition.hpp"
#include "Utility.hpp"
#include <iostream>
#include <queue>
#include <set>

using namespace std;

MinDenseTruss::MinDenseTruss(PUndirNet graph, TCnCom truss,
                             const std::vector<std::vector<int>> &keywords,
                             int trussness, const map<EId, int> &edgeTrussness)
    : graph(graph), truss(truss), keywords(keywords),
      edgeTrussness(edgeTrussness), trussness(trussness) {
    TIntV gDenNodes = truss.NIdV;
    gDenNodes.Sort();

    for (int i = 0; i < keywords.size(); ++i) {
        TIntV keywordV;
        for (int node : keywords[i]) {
            keywordV.Add(node);
        }
        keywordV.Sort();
        keywordV.Intrs(gDenNodes);
    }
}

TIntV MinDenseTruss::findKTruss(TIntV gdenNodes, const TIntV &toDelete) {
    PUndirNet subgraph = TUndirNet::New();
    for (int i = 0; i < gdenNodes.Len(); ++i) {
        int nodeId = gdenNodes[i];
        auto node = graph->GetNI(nodeId);
        if (!subgraph->IsNode(nodeId)) {
            subgraph->AddNode(nodeId);
        }
        for (int j = 0; j < node.GetDeg(); ++j) {
            auto neighbor = node.GetNbrNId(j);
            if (edgeTrussness.at(minmax_element(nodeId, neighbor)) ==
                    trussness &&
                gdenNodes.Count(neighbor) != 0) {
                if (!subgraph->IsNode(neighbor)) {
                    subgraph->AddNode(neighbor);
                }
                if (!subgraph->IsEdge(nodeId, neighbor)) {
                    subgraph->AddEdge(nodeId, neighbor);
                }
            }
        }
    }
    /// Equivalent to E_del in line 11, includes all edges to check
    set<EId> edgesToCheck;
    for (int i = 0; i < toDelete.Len(); ++i) {
        int src = toDelete[i];
        auto node = subgraph->GetNI(src);
        for (int j = 0; j < node.GetDeg(); ++j) {
            int dst = node.GetNbrNId(j);
            edgesToCheck.insert(minmax_element(src, dst));
        }
    }

    // Check every edge
    while (!edgesToCheck.empty()) {
        EId currEdge = *edgesToCheck.begin();
        edgesToCheck.erase(currEdge);
        auto [node1, node2] = currEdge;

        auto n1 = subgraph->GetNI(node1);
        auto n2 = subgraph->GetNI(node2);

        if (n1.GetDeg() > n2.GetDeg()) {
            swap(n1, n2);
        }

        for (int i = 0; i < n1.GetDeg(); ++i) {
            int neighbor = n1.GetNbrNId(i);
            if (n2.IsNbrNId(neighbor)) {
                auto edgeVw = subgraph->GetEI(node1, neighbor);
                auto edgeUw = subgraph->GetEI(node2, neighbor);
                int sup_uw = computeSupport(subgraph, edgeUw);
                int sup_vw = computeSupport(subgraph, edgeVw);
                sup_uw--;
                sup_vw--;
                if (sup_vw < trussness - 2) {
                    edgesToCheck.insert(minmax_element(node1, neighbor));
                }
                if (sup_uw < trussness - 2) {
                    edgesToCheck.insert(minmax_element(node2, neighbor));
                }
            }
        }

        subgraph->DelEdge(node1, node2);
    }

    for (auto beg = subgraph->BegNI(); beg != subgraph->EndNI(); beg++) {
        if (beg.GetDeg() == 0) {
            subgraph->DelNode(beg.GetId());
        }
    }

    TIntV ret;
    subgraph->GetNIdV(ret);
    set<int> nodes;
    for (int i = 0; i < ret.Len(); ++i) {
        nodes.insert(ret[i]);
    }
    // check if `ret` contains every keyword
    for (int i = 0; i < keywords.size(); ++i) {
        set<int> tmp = nodes;
        vector<int> tmp2;
        set_intersection(tmp.begin(), tmp.end(), keywords[i].begin(),
                         keywords[i].end(), back_inserter(tmp2));
        if (tmp2.empty()) {
            ret.Clr();
            return ret;
        }
    }
    return ret;
}

TIntV MinDenseTruss::selectNodesToDel(const TIntV &gdenDiffVis, int setting) {
    TIntV toDelete;

    if (setting & MinDenseTruss::Optimization::BatchDeletion) {
        if (!isLastSuccessful) {
            deleteSize = 1;
        } else {
            deleteSize += 1;
        }
        if (deleteSize > gdenDiffVis.Len()) {
            deleteSize = 1;
        }
        while (toDelete.Len() != deleteSize) {
            toDelete.AddUnique(gdenDiffVis.GetRndVal());
        }
    } else {
        toDelete.AddUnique(gdenDiffVis.GetRndVal());
    }

    return toDelete;
}

TIntV MinDenseTruss::localExploration() {
    // Extract subgraph from vertex list
    TIntV gdenNodes = truss.NIdV;
    PUndirNet subgraph = TUndirNet::New();
    for (int i = 0; i < gdenNodes.Len(); ++i) {
        for (int j = 0; j < gdenNodes.Len(); ++j) {
            int node1 = gdenNodes[i], node2 = gdenNodes[j];
            if (graph->IsEdge(node1, node2)) {
                if (!subgraph->IsNode(node1)) {
                    subgraph->AddNode(node1);
                }
                if (!subgraph->IsNode(node2)) {
                    subgraph->AddNode(node2);
                }
                if (!subgraph->IsEdge(node1, node2)) {
                    subgraph->AddEdge(node1, node2);
                }
            }
        }
    }

    // Prepare trussness for each node
    TrussDecomposition *trussDecom = new TrussDecomposition(subgraph);
    auto &nodeTrussness = trussDecom->getNodeTrussness();

    // Run BANKS algorithm to get Steiner tree
    BanksAlgorithm *banks = new BanksAlgorithm(subgraph, keywords);
    PUndirNet newGraph = banks->getAnswer();

    if (newGraph->GetNodes() == 0) {
        cout << "***For some reason, BANKS algorithm failed." << endl;
        TIntV result;
        graph->GetNIdV(result);
        return result;
    }

    // Incrementally add vertices
    TIntV nodesOfGraph;
    int addedNodes = 0;
    int threshold = steinerTreeThreshold;
    bool lastChanged = true;
    while (lastChanged) {
        lastChanged = false;
        // collect adj nodes and their trussness
        newGraph->GetNIdV(nodesOfGraph);
        /// {trussness, {adjacent node, original node}}
        priority_queue<pair<int, pair<int, int>>> queue;
        for (auto beg = newGraph->BegNI(); beg != newGraph->EndNI(); beg++) {
            int nodeId = beg.GetId();
            auto newBeg = graph->GetNI(nodeId);
            for (int i = 0; i < newBeg.GetDeg(); ++i) {
                int neighborId = newBeg.GetNbrNId(i);
                if (!newGraph->IsNode(neighborId)) {
                    if (nodeTrussness.find(neighborId) == nodeTrussness.end()) {
                        continue;
                    }
                    queue.push(
                        {edgeTrussness.at(minmax_element(neighborId, nodeId)),
                         {neighborId, nodeId}});
                } else {
                    if (!newGraph->IsEdge(neighborId, nodeId)) {
                        lastChanged = true;
                        newGraph->AddEdge(neighborId, nodeId);
                    }
                }
            }
        }
        // add adj nodes
        // note that nodes in the queue can be duplicate... and I plan to do
        // nothing about it
        while (!queue.empty() &&
               newGraph->GetNodes() < min(threshold, graph->GetNodes())) {
            auto edge = queue.top().second;
            auto added = edge.first, src = edge.second;
            queue.pop();
            if (!newGraph->IsNode(added)) {
                lastChanged = true;
                newGraph->AddNode(added);
            }
            if (!newGraph->IsEdge(src, added)) {
                lastChanged = true;
                newGraph->AddEdge(src, added);
            }
        }
        // find truss inside
        TrussDecomposition tDecom(newGraph);
        if (tDecom.getMaxTrussness() < trussness && lastChanged) {
            threshold += steinerTreeThreshold;
            continue;
        }
        const vector<TCnComV> &CCs = tDecom.getTrussCC();
        TIntV nodesV;
        // for every connected component, check if containing all keywords
        for (int i = 0; i < CCs[trussness].Len(); ++i) {
            nodesV = CCs[trussness][i].NIdV;
            vector<int> nodesVec;
            bool ok = true;
            for (int j = 0; j < nodesV.Len(); ++j) {
                nodesVec.push_back(nodesV[j]);
            }
            for (int j = 0; j < keywords.size(); ++j) {
                vector<int> tmp;
                sort(nodesVec.begin(), nodesVec.end());
                set_intersection(nodesVec.begin(), nodesVec.end(),
                                 keywords[j].begin(), keywords[j].end(),
                                 back_inserter(tmp));
                if (tmp.empty()) {
                    ok = false;
                    break;
                }
            }
            if (ok) {
                delete trussDecom;
                delete banks;
                return nodesV;
            } else {
                threshold += steinerTreeThreshold;
            }
        }
    }

    delete trussDecom;
    delete banks;
    cout << "***For some reason, BANKS algorithm failed. No valid result "
            "returned."
         << endl;
    TIntV result;
    graph->GetNIdV(result);
    return result;
}

TIntV MinDenseTruss::findMinDenseTruss(int setting) {
    if (setting & Optimization::LocalExploration) {
        return localExploration();
    }

    TIntV gdenNodes = truss.NIdV;
    TIntV gdenDiffVis = gdenNodes;
    TIntV visited;
    TIntV toDelete;
    int unsuccessfulDeletionCount = 0;

    while (gdenDiffVis.Len() != 0) {
        toDelete = selectNodesToDel(gdenDiffVis, setting);

        TIntV afterDelete = findKTruss(gdenNodes, toDelete);

        if (!afterDelete.Empty()) {
            gdenNodes = afterDelete;
            isLastSuccessful = true;
        } else {
            isLastSuccessful = false;
            if ((setting & Optimization::EarlyStopDeletion) &&
                ++unsuccessfulDeletionCount > earlyStopThreshold) {
                break;
            }
        }

        visited.AddVMerged(toDelete);
        gdenDiffVis = gdenNodes;
        gdenDiffVis.Sort();
        gdenDiffVis.Diff(visited);
    }

    return gdenNodes;
}