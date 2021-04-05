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
    int lastK = 0;
    int finalResult = 0;
    while (kMax >= kMin) {
        k = (kMax + kMin) / 2;

        // Equivalent to SC in line 4 in alg. 5, which stores the components
        // containing different keywords for current trussness
        vector<vector<int>> sc;
        for (int i = 0; i < keywords.size(); ++i) {
            if (!kwIndex.count(i)) {
                sc.push_back({});
            } else {
                if (!kwIndex.at(i).count(k)) {
                    sc.push_back({});
                } else {
                    sc.push_back(kwIndex.at(i).at(k));
                }
            }
        }

        lastIntersection = sc[0];
        vector<int> currIntersection;
        for (int i = 1; i < sc.size(); ++i) {
            set_intersection(lastIntersection.begin(), lastIntersection.end(),
                             sc[i].begin(), sc[i].end(),
                             back_inserter(currIntersection));

            if (currIntersection.empty()) {
                lastIntersection.clear();
                break;
            }

            swap(lastIntersection, currIntersection);
            currIntersection.clear();
        }

        if (lastIntersection.empty()) {
            kMax = k - 1;
        } else {
            kMin = k + 1;
            id = *min_element(lastIntersection.begin(), lastIntersection.end());
            result = index->getTrussCC().at(k)[id];
            finalResult = k;
        }
    }

    endTrussness = finalResult;
}