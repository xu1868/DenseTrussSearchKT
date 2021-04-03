#ifndef _UTILITY_H_
#define _UTILITY_H_

#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <queue>
#include <set>
#include <list>
#include <map>
#include <set>
#include <iterator>

#include "Snap.h"

#define WIN32
//#define _DEBUG_

const int Max_Update = 10000;

typedef std::pair<int, TIntV> nv_type;
typedef std::pair<int, int> eid_type;
typedef int vid_type;

inline void get_low_high_deg_vertices(const PUndirNet &net, const vid_type &src, const vid_type &dst, vid_type &low, vid_type &high) {
    if (net->GetNI(src).GetDeg() > net->GetNI(dst).GetDeg()) {
        low = dst;
        high = src;
    }
    else {
        low = src;
        high = dst;
    }
}

inline int compute_support(const PUndirNet &net, const PUndirNet::TObj::TEdgeI &EI) {
    int support = 0;
    vid_type u = -1, v = -1;

    vid_type src = EI.GetSrcNId();
    vid_type dst = EI.GetDstNId();
    get_low_high_deg_vertices(net, src, dst, u, v);

    for (int i = 0; i < net->GetNI(u).GetDeg(); i++) {
        vid_type w = net->GetNI(u).GetNbrNId(i);
        if (net->IsEdge(w, v))
            support += 1;
    }

    return support;
}

#ifdef _LINUX_
	#include <sys/time.h>
#endif

FILE *open_file(const char *file_name, const char *mode) ;

#endif
