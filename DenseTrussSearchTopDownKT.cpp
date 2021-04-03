#include "DenseTrussSearchTopDownKT.hpp"

using namespace std;

void DenseTrussSearchTopDownKT::topdownKT() {
    if (!index || !trussDecom)
        return;

    auto &kwMaxTrussN = index->getKeywordMaxTrussN();
    auto &kwIndex = index->getKeywordIndex();

    /// Bounds for the binary search
    int kMin = 2;
    int kMax = min_element(kwMaxTrussN.begin(), kwMaxTrussN.end(),
                           [](pair<int, int> p1, pair<int, int> p2) {
                               return p1.second < p2.second;
                           })
                   ->second;
    int id = 0;

    vector<int> lastIntersection;
    int k = 0;
    while (kMax > kMin) {
        k = (kMax + kMin) / 2;

        // Equivalent to SC in line 4 in alg. 5, which stores the components
        // containing different keywords for current trussness
        vector<vector<int>> sc;
        for (auto &kIndex : kwIndex) {
            sc.push_back(kIndex.second.at(k));
        }

        lastIntersection = sc[0];
        vector<int> currIntersection;
        for (int i = 1; i < sc.size(); ++i) {
            set_intersection(lastIntersection.begin(), lastIntersection.end(),
                             sc[i].begin(), sc[i].end(),
                             back_inserter(currIntersection));

            if (currIntersection.empty()) {
                break;
            }

            swap(lastIntersection, currIntersection);
            currIntersection.clear();
        }

        if (lastIntersection.empty()) {
            kMax = k;
        } else {
            kMin = k;
            if (kMax == kMin + 1) {
                break;
            }
        }
    }

    id = *min_element(lastIntersection.begin(), lastIntersection.end());
    result = index->getTrussCC().at(k)[id];
}