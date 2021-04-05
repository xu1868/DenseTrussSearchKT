#include "Utility.hpp"

using namespace std;

int computeSupport(const PUndirNet &net, const PUndirNet::TObj::TEdgeI &edge) {
    int support = 0;

    int u = edge.GetSrcNId();
    int v = edge.GetDstNId();
    if (net->GetNI(u).GetDeg() > net->GetNI(v).GetDeg()) {
        swap(u, v);
    }

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
