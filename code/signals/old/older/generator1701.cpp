#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <tuple>
#include <map>
#include <set>
#include <bitset>
#include <algorithm>
#include <limits>
#include <cfloat>
using namespace std;

int main() {

    vector<int> N {1, 2};
    vector<int> D {32, 64};
    vector<double> EPS {1, 2, 4, 8};
    vector<double> DEL {1, 2, 4, 8};

    int seed = 0;
    for (auto n : N) { 
        for (auto d : D) {
            for (auto eps : EPS) {
                for (auto del : DEL) {
                    vector<int> NUMEDGES;
                    int num = 1;
                    while (num < (ceil((d + eps) / del) + 1) / 4) {
                        NUMEDGES.push_back(num);
                        num = num * 2;
                    }

                    for (auto numEdges : NUMEDGES) {
                        for (int i = 0; i < 1; i++) {
                            bool done = false;
                            bool error = false;
                            
                            while (!done) {
                                mt19937 gen(seed);
                                uniform_real_distribution<> timeDist(0, d);
                                uniform_int_distribution<> valDist(0, 1);
                                
                                vector<map<double,double>> signalsTemp(n);

                                for (auto &sig : signalsTemp) {
                                    set<double> timeStamps;
                                    timeStamps.insert(0);

                                    for (int i = 0; i < numEdges; i++) {
                                        double temp = timeDist(gen);
                                        if  (timeStamps.find(temp) == timeStamps.end()) {
                                            timeStamps.insert(temp);
                                        }
                                        else {
                                            i--;
                                        }
                                    }

                                    int initVal = valDist(gen);
                                    int ctr = 0;
                                    for (const auto &t : timeStamps) {
                                        sig[t] = (initVal + ctr) % 2;
                                        ctr++;
                                    }
                                    
                                    /*double initVal = valDist(gen);
                                    sig[0] = valDist(gen);
                                    for (const auto &t : timeStamps) {
                                        sig[t] = valDist(gen);
                                    }*/        
                                }

                                vector<vector<pair<double,double>>> signals(n);

                                for (int i = 0; i < n; i++) {
                                    for (const auto &edge : signalsTemp[i]) {
                                        signals[i].push_back(make_pair(edge.first, edge.second));
                                    }
                                }

                                /* compute the uncertainty intervals */
                                vector<vector<vector<double>>> uncertainties(n);

                                for (int i = 0; i < n; i++) {
                                    int gi = signals[i].size();
                                    uncertainties[i].resize(gi);

                                    uncertainties[i][0].push_back(-del);
                                    for (int j = 1; j < gi; j++) {
                                        double tj = signals[i][j].first;
                                        double low = max(uncertainties[i][j - 1][0] + del, tj - eps);
                                        uncertainties[i][j].push_back(low);
                                    }

                                    uncertainties[i][gi - 1].push_back(signals[i][gi - 1].first + eps);
                                    for (int j = gi - 2; j > 0; j--) {
                                        double tj = signals[i][j].first;
                                        double high = min(uncertainties[i][j + 1][1] - del, tj + eps);
                                        uncertainties[i][j].push_back(high);

                                        // check input validity
                                        if (uncertainties[i][j][0] >= uncertainties[i][j][1]) {
                                            error = true;
                                            break;
                                        }
                                    }
                                    
                                    if (error) {
                                        break;
                                    }
                                }

                                seed++;
                                if (!error) {
                                    cout << "success: " << n << " " << d << " " << eps << " " << del << " " << numEdges << endl;
                                    done = true;
                                }
                            }
                            
                            /* write the generated signals to a file */
                            /* problem: dense valid signals are difficult to generate randomly */
                        }
                    }
                }
            }
        }
    }

    return 0;
}

