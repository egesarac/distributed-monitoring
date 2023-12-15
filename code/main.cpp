#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <tuple>
#include <map>
#include <set>
#include <algorithm>
//#include <boost>
using namespace std;

int main() {
    
    /* get spec */ 
    // TODO
    double eps = 2;
    double del = 2;

    /* generate random signals */ 
    int n = 2;
    int d = 10;
    int maxEdges = ceil((d + eps) / del) + 1; // per signal, including the initial value
    double minVal = -10;
    double maxVal = 10;

    //random_device rd;  // a seed source for the random number engine
    //mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
    int seed = 1234;
    mt19937 gen(seed);

    uniform_int_distribution<> edgeDist(1, maxEdges);
    uniform_int_distribution<> timeDist(0, d - 1);
    uniform_int_distribution<> valDist(0, 1);
    //uniform_real_distribution<> timeDist(0, d);
    //uniform_real_distribution<> valDist(minVal, maxVal);

    vector<map<int,int>> signals(n);

    for (auto &sig : signals) {
        set<int> timeStamps;
        timeStamps.insert(0);

        int numEdges = edgeDist(gen);
        for (int i = 0; i < numEdges; i++) {
            int temp = timeDist(gen);
            if  (timeStamps.find(temp) == timeStamps.end()) {
                timeStamps.insert(temp);
            }
            else {
                i--;
            }
        }

        int initVal = valDist(gen);
        int ctr = 0;
        for (auto t : timeStamps) {
            sig[t] = (initVal + ctr) % 2;
            ctr++;
        }
    }

    /* compute the uncertainty intervals */
    vector<vector<vector<int>>> uncertainties(n);

    int i = 0;
    for (auto sig : signals) {
        uncertainties[i].resize(sig.size());

        int j = 0;
        uncertainties[i][j].push_back((-1) * del);

        map<int,int>::iterator it = ++sig.begin();
        j++;
        for (; it != sig.end(); it++) {
            int tj = it->first; 
            int low = max(uncertainties[i][j-1][0] + del, tj - eps);
            uncertainties[i][j].push_back(low);
            j++;
        }

        j = sig.size() - 1;
        map<int,int>::reverse_iterator rit = sig.rbegin();
        uncertainties[i][j].push_back(rit->first + eps);

        j--;
        rit++;
        for (; next(rit) != sig.rend(); rit++) {
            int tj = rit->first; 
            int high = min(uncertainties[i][j+1][1] - del, tj + eps);
            uncertainties[i][j].push_back(high);
            j--;
        }

        i++;
    }

    /* compute the canonical segmentation */ 
    set<int> segmentation;

    for (auto s : uncertainties) { // over signals
        for (int j = 1; j < s.size(); j++) { // over edges
            for (auto b : s[j]) { // over uncertainty bounds
                segmentation.insert(b);
            }
        }
    }

    /* compute the value expressions (assuming for now the signals are boolean) */
    


    return 0;
}

