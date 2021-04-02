#include "Utility.hpp"

int computeSupport(const PUndirNet &net, const PUndirNet::TObj::TEdgeI &edge) {
    int support = 0;

    auto [u, v] = minMaxDegVertices(net, edge.GetDstNId(), edge.GetSrcNId());

    for (int i = 0; i < net->GetNI(u).GetDeg(); ++i) {
        VId w = net->GetNI(u).GetNbrNId(i);
        if (net->IsEdge(w, v)) {
            ++support;
        }
    }

    return support;
}

FILE *openFile(const char *file_name, const char *mode) {
    FILE *fin = fopen(file_name, mode);
    if (fin == NULL) {
        printf("Can not open file: %s\n", file_name);
        exit(1);
    }

    return fin;
}
