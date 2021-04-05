/**
 * @file main.cpp
 * @author Tianyang Xu
 * @brief This file is used to test the classes, also serving as a tutorial as
 * to how to use these classes.
 * @version 0.1
 * @date 2021-04-04
 *
 *
 */

#include "DenseTrussSearchTopDown.hpp"
#include "DenseTrussSearchTopDownKT.hpp"
#include "KTIndex.hpp"
#include "MinDenseTruss.hpp"
#include "TrussDecomposition.hpp"
#include "Utility.hpp"
#include <iostream>

using namespace std;

PUndirNet genRandomNetwork(int nodes, int edges) {
    PUndirNet ret = PUndirNet::New();
    for (int i = 0; i < nodes; ++i) {
        ret->AddNode(i);
    }
    while (ret->GetEdges() < edges) {
        int i = rand() % nodes;
        int j = rand() % nodes;
        if (i != j && !ret->IsEdge(i, j)) {
            ret->AddEdge(i, j);
        }
    }
    return ret;
}

vector<vector<int>> genRandomKeywords(PUndirNet graph, int kwCount) {
    vector<vector<int>> ret(kwCount);

    TIntV nodeVec;
    graph->GetNIdV(nodeVec);
    nodeVec.Shuffle(TInt::Rnd);

    vector<int> indexVec;

    while (true) {
        indexVec.clear();
        for (int i = 0; i < nodeVec.Len(); ++i) {
            indexVec.push_back(i);
        }
        random_shuffle(indexVec.begin(), indexVec.end());
        sort(indexVec.begin(), indexVec.begin() + kwCount - 1);
        indexVec.resize(kwCount - 1);
        indexVec.push_back(nodeVec.Len());
        bool flag = true;
        for (int i = 0; i < indexVec.size(); ++i) {
            if ((i == 0 && indexVec[i] == 0) ||
                (i != 0 && indexVec[i] == indexVec[i - 1])) {
                flag = false;
                break;
            }
        }
        if (flag) {
            break;
        } else {
            continue;
        }
    }

    int last = 0;
    for (int i = 0; i < indexVec.size(); ++i) {
        for (int j = last; j < indexVec[i]; ++j) {
            ret[i].push_back(nodeVec[j]);
        }
        last = indexVec[i];
    }

    // ! Keywords must be *SORTED* unless some classes will *NOT* work!
    for (auto &v : ret) {
        sort(v.begin(), v.end());
    }

    return ret;
}

int testTrussDecomposition(PUndirNet graph) {
    // Assuming `computeSupport` in `Utility.hpp` works fine
    cout << "===Performing truss decomposition..." << endl;
    TrussDecomposition trussDecomp = TrussDecomposition(graph);
    auto &trussSubgraph = trussDecomp.getTrussSubgraph();
    cout << "===Truss decomposition finished." << endl;
    for (int i = 0; i < trussSubgraph.size(); ++i) {
        cout << "======Trussness " << i << ": " << trussSubgraph[i]->GetEdges()
             << " edges." << endl;
        for (auto beg = trussSubgraph[i]->BegEI();
             beg != trussSubgraph[i]->EndEI(); beg++) {
            int sup = computeSupport(graph, beg);
            if (sup < i - 2) {
                cout << "!!!ERROR! Not a " << i << "-truss. EXITING...";
                return -1;
            }
        }
    }

    return 0;
}

int testTrussSearch(PUndirNet graph, const vector<vector<int>> &keywords) {
    cout << "===Performing DenseTrussSearchTopDown..." << endl;
    DenseTrussSearchTopDown *topDown =
        new DenseTrussSearchTopDown(graph, keywords);

    auto cnCom = topDown->getResult();
    int trussness = topDown->getTrussness();

    if (cnCom.Len() == 0) {
        cout << "***No valid solution found. This is because some keywords "
                "are only contained by nodes not connected to other nodes, "
                "which is possible in completely random graphs. Exiting."
             << endl;
        return 0;
    }
    // reconstruct subgraph from TCnCom
    TrussDecomposition trussDecomp = TrussDecomposition(graph);
    auto &edgeTrussness = trussDecomp.getEdgeTrussness();
    PUndirNet subgraph = TUndirNet::New();
    for (int i = 0; i < cnCom.Len(); ++i) {
        int src = cnCom[i];
        auto node = graph->GetNI(src);
        for (int j = 0; j < node.GetDeg(); ++j) {
            int nbr = node.GetNbrNId(j);
            if (edgeTrussness.at(minmax_element(src, nbr)) >= trussness) {
                if (!subgraph->IsNode(src)) {
                    subgraph->AddNode(src);
                }
                if (!subgraph->IsNode(nbr)) {
                    subgraph->AddNode(nbr);
                }
                if (!subgraph->IsEdge(src, nbr)) {
                    subgraph->AddEdge(src, nbr);
                }
            }
        }
    }
    cout << "======Connected component: " << cnCom.Len() << " nodes." << endl;
    cout << "======Connected component: " << subgraph->GetEdges() << " edges."
         << endl;
    cout << "======Trussness: " << trussness << endl;

    // check if contains all keywords
    set<int> nodes;
    for (int i = 0; i < cnCom.Len(); ++i) {
        nodes.insert(cnCom[i]);
    }
    for (int i = 0; i < keywords.size(); ++i) {
        vector<int> tmp;
        set_intersection(nodes.begin(), nodes.end(), keywords[i].begin(),
                         keywords[i].end(), back_inserter(tmp));
        if (tmp.empty()) {
            cout << "!!!ERROR! not containing keyword " << i << ". EXITING..."
                 << endl;
            return -1;
        }
    }
    cout << "===DenseTrussSearchTopDown finished." << endl;
    return 0;
}

// Yes I'm repeating myself here. So what?... It's only a test case.
// ...Maybe I should make an interface but I'm way too lazy.
int testTrussSearchKT(PUndirNet graph, const vector<vector<int>> &keywords) {
    cout << "===Performing DenseTrussSearchTopDownKT..." << endl;
    DenseTrussSearchTopDownKT *topDown =
        new DenseTrussSearchTopDownKT(graph, keywords);

    auto cnCom = topDown->getResult();
    int trussness = topDown->getTrussness();

    if (cnCom.Len() == 0) {
        cout << "***No valid solution found. This is because some keywords "
                "are only contained by nodes not connected to other nodes, "
                "which is possible in completely random graphs. Exiting."
             << endl;
        return 0;
    }
    // reconstruct subgraph from TCnCom
    TrussDecomposition trussDecomp = TrussDecomposition(graph);
    auto &edgeTrussness = trussDecomp.getEdgeTrussness();
    PUndirNet subgraph = TUndirNet::New();
    for (int i = 0; i < cnCom.Len(); ++i) {
        int src = cnCom[i];
        auto node = graph->GetNI(src);
        for (int j = 0; j < node.GetDeg(); ++j) {
            int nbr = node.GetNbrNId(j);
            if (edgeTrussness.at(minmax_element(src, nbr)) >= trussness) {
                if (!subgraph->IsNode(src)) {
                    subgraph->AddNode(src);
                }
                if (!subgraph->IsNode(nbr)) {
                    subgraph->AddNode(nbr);
                }
                if (!subgraph->IsEdge(src, nbr)) {
                    subgraph->AddEdge(src, nbr);
                }
            }
        }
    }
    cout << "======Connected component: " << subgraph->GetNodes() << " nodes."
         << endl;
    cout << "======Connected component: " << subgraph->GetEdges() << " edges."
         << endl;
    cout << "======Trussness: " << trussness << endl;

    // check if contains all keywords
    vector<int> nodes;
    for (int i = 0; i < cnCom.Len(); ++i) {
        nodes.push_back(cnCom[i]);
    }
    sort(nodes.begin(), nodes.end());
    for (int i = 0; i < keywords.size(); ++i) {
        vector<int> tmp;
        vector<int> kwds = keywords[i];
        sort(kwds.begin(), kwds.end());
        set_intersection(nodes.begin(), nodes.end(), kwds.begin(), kwds.end(),
                         back_inserter(tmp));
        if (tmp.empty()) {
            cout << "!!!ERROR! not containing keyword " << i << ". EXITING..."
                 << endl;
            return -1;
        }
    }

    cout << "===DenseTrussSearchTopDownKT finished." << endl;
    cout << "===Performing MinDenseTruss..." << endl;
    MinDenseTruss minDense =
        MinDenseTruss(graph, cnCom, keywords, trussness, edgeTrussness);

    auto minDenseRet = minDense.findMinDenseTruss(
        MinDenseTruss::EarlyStopDeletion | MinDenseTruss::BatchDeletion);
    cout << "======MinDenseTruss: " << minDenseRet.Len() << " nodes." << endl;

    // check if contains all keywords
    vector<int> nodes2;
    for (int i = 0; i < minDenseRet.Len(); ++i) {
        nodes2.push_back(minDenseRet[i]);
    }
    sort(nodes2.begin(), nodes2.end());
    for (int i = 0; i < keywords.size(); ++i) {
        vector<int> tmp;
        vector<int> kwds = keywords[i];
        sort(kwds.begin(), kwds.end());
        set_intersection(nodes2.begin(), nodes2.end(), kwds.begin(), kwds.end(),
                         back_inserter(tmp));
        if (tmp.empty()) {
            cout << "!!!ERROR! not containing keyword " << i << ". EXITING..."
                 << endl;
            return -1;
        }
    }

    // finally check LocalSearch
    // auto minDenseRet2 =
    //     minDense.findMinDenseTruss(MinDenseTruss::LocalExploration);
    return 0;
}

int test(PUndirNet graph, const vector<vector<int>> &keywords,
         const string &name) {
    cout << "Testing on " << name << "..." << endl;
    cout << "Testing truss decomposition..." << endl;
    if (testTrussDecomposition(graph) != 0) {
        cout << "!!!TESTING " << name << " FAILED FOR TRUSS DECOMPOSITION!!!"
             << endl;
        return -1;
    }
    if (testTrussSearch(graph, keywords) != 0) {
        cout << "!!!TESTING " << name << " FAILED FOR TRUSS SEARCH!!!" << endl;
        return -1;
    }
    if (testTrussSearchKT(graph, keywords) != 0) {
        cout << "!!!TESTING " << name << " FAILED FOR TRUSS SEARCHKT!!!"
             << endl;
        return -1;
    }
    cout << "Testing " << name << " finished." << endl;
    return 0;
}

int main() {
    srand(time(NULL));
    cout << "Start testing..." << endl;

    PUndirNet graph1 = genRandomNetwork(10, 15);
    PUndirNet graph2 = genRandomNetwork(100, 200);
    PUndirNet graph3 = genRandomNetwork(1000, 20000);
    auto kw1 = genRandomKeywords(graph1, 3);
    auto kw2 = genRandomKeywords(graph2, 3);
    auto kw3 = genRandomKeywords(graph1, 3);

    if (test(graph1, kw1, "Graph 1") == -1) {
        return -1;
    }
    if (test(graph2, kw2, "Graph 2") == -1) {
        return -1;
    }
    if (test(graph3, kw3, "Graph 3") == -1) {
        return -1;
    }

    return 0;
}