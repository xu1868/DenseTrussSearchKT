#include "BanksAlgorithm.hpp"
#include <queue>
#include <set>

using namespace std;

void BanksAlgorithm::initIter(int nodeId) {
    /*
     * Note: I am going to use BFS here though the paper suggests Dijkstra,
     * because here I'm going to assign weight 1 to every edge.
     */
    BanksIterator iter;
    iter.nodeId = nodeId;

    auto node = graph->GetNI(nodeId);
    iter.bfsTree = TNGraph::New();
    iter.bfsTree->AddNode(node.GetId());

    queue<decltype(node)> queue;
    queue.push(node);
    map<int, int> distances;
    distances[nodeId] = 0;
    set<int> visited;
    visited.insert(node.GetId());

    while (!queue.empty()) {
        auto currNode = queue.front();
        queue.pop();

        for (int i = 0; i < currNode.GetDeg(); ++i) {
            int neighborNodeId = currNode.GetNbrNId(i);
            auto neighborNode = graph->GetNI(neighborNodeId);
            visited.insert(neighborNodeId);

            if (!visited.count(neighborNodeId)) {
                queue.push(neighborNode);
            }

            if (!iter.bfsTree->IsNode(neighborNodeId)) {
                iter.bfsTree->AddNode(neighborNodeId);
            }
            if (!iter.bfsTree->IsEdge(neighborNodeId, currNode.GetId())) {
                iter.bfsTree->AddEdge(neighborNodeId, currNode.GetId());
            }

            iter.distsAndNodes.insert(
                {distances[currNode.GetId()] + 1, neighborNodeId});
        }
    }

    iteratorHeap.push(iter);
}

void BanksAlgorithm::consTree(int rootNode, const std::vector<int> &tuple) {
    PUndirNet tree = TUndirNet::New();
    tree->AddNode(rootNode);
    for (int i = 0; i < tuple.size(); ++i) {
        auto leafAndPath = nodeLists[rootNode][i][tuple[i]];
        auto &path = leafAndPath.second;
        int last = rootNode;
        for (int n : path) {
            if (!tree->IsNode(n)) {
                tree->AddNode(n);
            }
            if (n != last) {
                tree->AddEdge(last, n);
            }
            last = n;
        }
    }
    if (tree->GetNI(rootNode).GetDeg() != 1) {
        outputHeap.push(tree);
    }
}

void BanksAlgorithm::runBanks() {
    // initialize nodeToKeyword
    for (int i = 0; i < keywords.size(); ++i) {
        for (int j = 0; j < keywords[i].size(); ++j) {
            nodeToKeyword[keywords[i][j]] = i;
        }
    }
    // initialize iterators
    for (auto beg = graph->BegNI(); beg != graph->EndNI(); beg++) {
        initIter(beg.GetId());
    }

    while (!iteratorHeap.empty()) {
        auto currIter = iteratorHeap.top();
        iteratorHeap.pop();
        int currKeyword = nodeToKeyword[currIter.nodeId];

        auto nextDistAndNode = *currIter.distsAndNodes.begin();
        currIter.distsAndNodes.erase(nextDistAndNode);
        if (!currIter.distsAndNodes.empty()) {
            iteratorHeap.push(currIter);
        }

        auto [nextDist, nextNode] = nextDistAndNode;

        if (nodeLists.find(nextNode) == nodeLists.end()) {
            nodeLists[nextNode] =
                vector<vector<pair<int, vector<int>>>>(keywords.size());
        }

        // find path from nextNode to currIter
        auto &bfsTree = currIter.bfsTree;
        vector<int> path;

        auto startNode = bfsTree->GetNI(nextNode);
        while (startNode.GetId() != currIter.nodeId) {
            path.push_back(startNode.GetId());
            int neighbor = startNode.GetOutNId(0);
            startNode = bfsTree->GetNI(neighbor);
        }
        path.push_back(currIter.nodeId);
        nodeLists[nextNode][currKeyword].push_back({currIter.nodeId, path});

        // find CrossProduct
        int allCombinations = 1;
        vector<int> tupleNumber(keywords.size());
        vector<int> tupleNumberToConsTree(keywords.size(), 0);
        for (int i = 0; i < keywords.size(); ++i) {
            tupleNumber.push_back(nodeLists[nextNode][i].size());
            allCombinations *= nodeLists[nextNode][i].size();
        }
        for (int j = 0; j < allCombinations; ++j) {
            tupleNumberToConsTree.back()++;
            for (int i = tupleNumberToConsTree.size() - 1; i >= 0; --i) {
                if (tupleNumberToConsTree[i] > tupleNumber[i]) {
                    tupleNumberToConsTree[i] = 0;
                    if (i == 0) {
                        break;
                    } else {
                        tupleNumberToConsTree[i - 1]++;
                    }
                }
            }
            consTree(nextNode, tupleNumberToConsTree);

            if (outputHeap.size() == maxOutputHeapSize) {
                return;
            }
        }
    }
}