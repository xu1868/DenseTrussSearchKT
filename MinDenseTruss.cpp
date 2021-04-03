#include "MinDenseTruss.hpp"
#include "Utility.hpp"
#include <set>

using namespace std;

TIntV MinDenseTruss::findKTruss(TIntV gdenNodes, const TIntV &toDelete) {
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
    /// Equivalent to E_del in line 11, includes all edges to check
    set<EId> edgesToCheck;
    for (int i = 0; i < gdenNodes.Len(); ++i) {
        int src = gdenNodes[i];
        auto node = subgraph->GetNI(src);
        for (int j = 0; j < node.GetDeg(); ++j) {
            int dst = node.GetNbrNId(j);
            edgesToCheck.insert({src, dst});
        }
    }
    // Perform truss decomposition on the subgraph
    TrussDecomposition *trussDecom = new TrussDecomposition(subgraph);
    auto edgeSupport = trussDecom->getEdgeSupport();
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
                int sup_vw =
                    (edgeSupport[minmax_element(node1, neighbor)] -= 1);
                int sup_uw =
                    (edgeSupport[minmax_element(node2, neighbor)] -= 1);
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
    delete trussDecom;
    return ret;
}

TIntV MinDenseTruss::selectNodesToDel(const TIntV &gdenDiffVis, int setting) {
    TIntV toDelete;

    if (setting | MinDenseTruss::Optimization::BatchDeletion) {
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

TIntV MinDenseTruss::findMinDenseTruss(int setting) {
    TIntV gdenNodes = truss.NIdV;
    TIntV gdenDiffVis = gdenNodes;
    TIntV visited;
    TIntV toDelete;
    int unsuccessfulDeletionCount = 0;

    while (!gdenDiffVis.Empty()) {
        toDelete = selectNodesToDel(gdenDiffVis, setting);

        TIntV afterDelete = findKTruss(gdenNodes, toDelete);

        if (!afterDelete.Empty()) {
            gdenNodes = afterDelete;
            isLastSuccessful = true;
        } else {
            isLastSuccessful = false;
            if ((setting | Optimization::EarlyStopDeletion) &&
                ++unsuccessfulDeletionCount > earlyStopThreshold) {
                break;
            }
        }

        visited.AddVMerged(toDelete);
        gdenDiffVis = gdenNodes;
        gdenDiffVis.Diff(afterDelete);
    }

    return gdenNodes;
}