#include "MinDenseTruss.hpp"
#include "TrussDecomposition.hpp"
#include "Utility.hpp"
#include "lib/local_search.h"
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
        QInComV.push_back(keywordV);
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
    BanksAlgorithm *banks = new BanksAlgorithm();
    banks->run(subgraph, QInComV, BASIC_STEINER_TREE, true);

    // Transfer the Steiner tree to our graph
    auto tree = banks->g;
    PUndirNet newGraph = PUndirNet::New();
    for (int i = 0; i < tree->n; ++i) {
        for (int j = 0; j < tree->nodes[i].len; ++j) {
            int neighbor = tree->nodes[i].adj[j];
            if (!newGraph->IsNode(i)) {
                newGraph->AddNode(i);
            }
            if (!newGraph->IsNode(j)) {
                newGraph->AddNode(j);
            }
            if (!newGraph->IsEdge(i, j)) {
                newGraph->AddEdge(i, j);
            }
        }
    }

    // Incrementally add vertices
    TIntV nodesOfGraph;
    int addedNodes = 0;
    int threshold = steinerTreeThreshold;
    while (true) {
        // collect adj nodes and their trussness
        newGraph->GetNIdV(nodesOfGraph);
        /// {trussness, {adjacent node, original node}}
        priority_queue<pair<int, pair<int, int>>> queue;
        for (auto beg = newGraph->BegNI(); beg != newGraph->EndNI(); beg++) {
            int nodeId = beg.GetId();
            for (int i = 0; i < beg.GetDeg(); ++i) {
                int neighborId = beg.GetNbrNId(i);
                if (!newGraph->IsNode(neighborId)) {
                    queue.push(
                        {nodeTrussness.at(neighborId), {neighborId, nodeId}});
                }
            }
        }
        // add adj nodes
        // note that nodes in the queue can be duplicate... and I plan to do
        // nothing about it
        while (newGraph->GetNodes() < min(threshold, truss.Len())) {
            auto edge = queue.top().second;
            auto added = edge.first, src = edge.second;
            queue.pop();
            if (!newGraph->IsNode(added)) {
                newGraph->AddNode(added);
            }
            if (!newGraph->IsEdge(src, added)) {
                newGraph->AddEdge(src, added);
            }
        }
        // find truss inside
        TrussDecomposition tDecom(newGraph);
        if (tDecom.getMaxTrussness() < trussness) {
            threshold += steinerTreeThreshold;
            continue;
        }
        const vector<TCnComV> &CCs = tDecom.getTrussCC();
        // for every connected component, check if containing all keywords
        for (int i = 0; i < CCs[trussness].Len(); ++i) {
            TIntV nodesV = CCs[trussness][i].NIdV;
            vector<int> nodesVec;
            bool ok = true;
            for (int j = 0; j < nodesV.Len(); ++j) {
                nodesVec.push_back(nodesV[j]);
            }
            for (int j = 0; j < keywords.size(); ++j) {
                vector<int> tmp;
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
            }
        }

        if (graph->GetNodes() == truss.Len()) {
            break;
        }
    }

    delete trussDecom;
    delete banks;
    return TIntV();
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

        // cout << "gdenDIffVis:" << endl;
        // for (int i = 0; i < gdenDiffVis.Len(); ++i) {
        //     cout << gdenDiffVis[i] << " ";
        // }
        // cout << endl;

        // cout << "gdenNodes:" << endl;
        // for (int i = 0; i < gdenNodes.Len(); ++i) {
        //     cout << gdenNodes[i] << " ";
        // }
        // cout << endl;

        // cout << "visited:" << endl;
        // for (int i = 0; i < visited.Len(); ++i) {
        //     cout << visited[i] << " ";
        // }
        // cout << endl;
    }

    return gdenNodes;
}