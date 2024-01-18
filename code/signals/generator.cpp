#include <iostream>
#include <fstream>
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
    vector<int> D {256, 512, 1024};
    vector<double> EPS {1, 2, 4, 8};
    vector<double> DEL {1, 2, 4, 8};

    for (const auto &n : N) { 
        for (const auto &d : D) {
            for (const auto &eps : EPS) {
                for (const auto &del : DEL) {
                    vector<int> NUMEDGES;
                    int num = 1;
                    while (num <= ceil((d + eps) / del) + 1) {
                        NUMEDGES.push_back(num);
                        num = num * 2;
                    }
                    
                    for (const auto &numEdges : NUMEDGES) {
                        bool error = false;
                        vector<double> timeStamps;
                        double tt = 0;
                        while (tt < d) {
                            timeStamps.push_back(tt);
                            tt += double(d) / (double(numEdges) + 1.0);
                        }

                        random_device rd;
                        mt19937 gen(rd());
                        uniform_int_distribution<> valDist(0, 1);
                        int initVal = valDist(gen);
                        int ctr = 0;
                        map<double,double> sig;
                        for (const auto &t : timeStamps) {
                            sig[t] = (initVal + ctr) % 2;
                            ctr++;
                        }

                        vector<pair<double,double>> signal;
                        for (const auto &edge : sig) {
                            signal.push_back(make_pair(edge.first, edge.second));
                        }

                        vector<vector<double>> uncertainties;

                        int gi = signal.size();
                        uncertainties.resize(gi);

                        uncertainties[0].push_back(-del);
                        for (int j = 1; j < gi; j++) {
                            double tj = signal[j].first;
                            double low = max(uncertainties[j - 1][0] + del, tj - eps);
                            uncertainties[j].push_back(low);
                        }

                        uncertainties[gi - 1].push_back(signal[gi - 1].first + eps);
                        for (int j = gi - 2; j > 0; j--) {
                            double tj = signal[j].first;
                            double high = min(uncertainties[j + 1][1] - del, tj + eps);
                            uncertainties[j].push_back(high);
                        }

                        for (int j = 1; j < gi; j++) {
                            if (uncertainties[j][0] >= uncertainties[j][1]) {
                                error = true;
                                break;
                            }
                        }

                        if (!error) {
                            ofstream sigdata;
                            string filename = "data/" + to_string(d) + "_" + to_string(int(eps)) + "_" + to_string(int(del)) + "_" + to_string(numEdges) + "_" + to_string(n) + ".txt";
                            sigdata.open(filename);

                            for (const auto &p : sig) {
                                sigdata << p.first << " " << p.second << endl;
                            }

                            sigdata.close();
                        }
                        else {
                            string filename = "data/" + to_string(d) + "_" + to_string(int(eps)) + "_" + to_string(int(del)) + "_" + to_string(numEdges) + "_" + to_string(n) + ".txt";
                            cout << "error: " + filename << endl;
                        }
                    }   
                }
            }
        }
    }

    return 0;
}

