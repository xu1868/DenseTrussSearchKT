/**
 * @file Utility.hpp
 * @author Tianyang Xu
 * @brief This file includes some utilities widely used in algorithms in this
 * paper, such as computing support and trussness.
 * @version 0.1
 * @date 2021-04-02
 *
 */

#pragma once

#include <Snap.h>
#include <algorithm>
#include <cstdio>
#include <utility>

/// Type of vertex ID
typedef int VId;
/// Type of edge
typedef std::pair<VId, VId> EId;
/// Type of a node with a vector
typedef std::pair<int, TIntV> NVec;

/**
 * @brief This function returns a pair of vertices sorted ascendingly by their
 * degrees.
 *
 * @param net The graph where the vertices lie.
 * @param v1 Vertex one or whatever.
 * @param v2 Another vertex.
 * @return std::pair<VId, VId> A pair of vertices sorted ascendingly by their
 * degrees.
 */
inline std::pair<VId, VId> minMaxDegVertices(const PUndirNet &net, VId v1,
                                             VId v2) {
    int deg1 = net->GetNI(v1).GetDeg();
    int deg2 = net->GetNI(v2).GetDeg();
    return (deg1 < deg2) ? std::make_pair(v1, v2) : std::make_pair(v2, v1);
}

/**
 * @brief This function returns the support of an edge in an undirected network.
 *
 * @param net
 * @param edge
 * @return int The support.
 */
int computeSupport(const PUndirNet &net, const PUndirNet::TObj::TEdgeI &edge);

/**
 * @brief fopen with error detection.
 *
 * @param fileName
 * @param mode
 * @return FILE*
 */
FILE *openFile(const char *fileName, const char *mode);

inline std::pair<int, int> minmax_element(int ele1, int ele2) {
    return ele1 < ele2 ? std::make_pair(ele1, ele2)
                       : std::make_pair(ele2, ele1);
}

void outputGraph(PUndirNet graph);