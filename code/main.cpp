#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <tuple>
#include <map>
#include <set>
#include <bitset>
#include <algorithm>
//#include <boost/dynamic_bitset.hpp>
using namespace std;
//using namespace boost;

#define SIZE 8



vector<bitset<SIZE>> bitsetConjunction(const vector<bitset<SIZE>> &v1, const vector<bitset<SIZE>> &v2) {
    vector<bitset<SIZE>> vv(2);

    // get max length for types 00 01 10 11 in v1 and v2, find max length in their conjunction, set the appropriate smaller lengths in vv, check 1 separately

    bitset<SIZE> evenMask; // set all even bits
    bitset<SIZE> oddMask; // set all odd bits

    vector<int> len1(4);

    // len1[0] = largest even 1-index of v1[0] - 1
    // len1[1] = largest odd 1-index of v1[0] - 1
    // len1[2] = largest even 1-index of v1[1] - 1
    // len1[3] = largest odd 1-index of v1[1] - 1
    // careful with zero-length cases
    // same for len2

    // len3 for the result, pairs correspond to (len1[.], len2[.]) 
    // 0 : max of 00 01 02 03 10 12 20 21 30, minus 1
    // 1 : max of 11 13 31, minus 1
    // 2 : max of 22 23 32, minus 1
    // 3 : 33 minus 1, check for vv[1][0] separately

    // set vv[0][len3[0]] and all vv[0][i] for smaller i of the same parity
    // same for the rest, except for vv[1][0]

    return vv;
}

vector<bitset<SIZE>> bitsetNegation(const vector<bitset<SIZE>> &v) {
    vector<bitset<SIZE>> vv(2);

    vv[0] = v[1];
    vv[1] = v[0];

    return vv;
}

void convertIntoBitset(vector<vector<vector<bitset<SIZE>>>> &aps, vector<vector<set<string>>> &valExprs) {
    aps.resize(valExprs.size());

    for (int i = 0; i < valExprs.size(); i++) {
            aps[i].resize(valExprs[i].size());

            for (int j = 0; j < valExprs[i].size(); j++) {
                aps[i][j].resize(2);
                
                for (auto s : valExprs[i][j]) {
                    if (!s.empty()) {
                        string temp = s;
                        temp.erase(remove(temp.begin(), temp.end(), ';'), temp.end());
                        
                        int k = (temp[0] == '0') ? 0 : 1;
                        int l = temp.length() - 1;
                        aps[i][j][k].set(l, true);
                    }
                }
            }
        }
}

set<string> concatWithDestutter(const set<string> &set1, const set<string> &set2) {
    set<string> result;

    for (const auto &s1 : set1) {
        for (const auto &s2 : set2) {
            if (!s1.empty() && !s2.empty()) {
                if (s1.rfind(';') == string::npos && s2.find(';') == string::npos) {
                    if (s1 != s2) {
                        result.insert(s1 + ";" + s2); 
                    }
                    else {
                        result.insert(s1);
                    }
                }
                else if (s1.rfind(';') == string::npos && s2.find(';') != string::npos) {
                    if (s1 != s2.substr(0, s2.find(';'))) {
                        result.insert(s1 + ";" + s2); 
                    }
                    else {
                        result.insert(s2);
                    }
                }
                else if (s1.rfind(';') != string::npos && s2.find(';') == string::npos) {
                    if (s1.substr(s1.rfind(';') + 1, s1.length()) != s2) {
                        result.insert(s1 + ";" + s2); 
                    }
                    else {
                        result.insert(s1);
                    }
                }
                else {
                    if (s1.substr(s1.rfind(';') + 1, s1.length()) != s2.substr(0, s2.find(';'))) {
                        result.insert(s1 + ";" + s2); 
                    }
                    else {
                        result.insert(s1 + ";" + s2.substr(s2.find(';') + 1, s2.length()));
                    }
                }
            }
            else {
                if (!s1.empty()) {
                    result.insert(s1);
                }
                else {
                    result.insert(s2);
                }
            }
        }
    }

    return result;
}


int main() {
    
    /* get spec */ 
    // TODO
    int m = 4;
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

    vector<map<int,int>> signalsTemp(n);

    for (auto &sig : signalsTemp) {
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
        for (const auto &t : timeStamps) {
            sig[t] = (initVal + ctr) % 2;
            ctr++;
        }
    }

    vector<vector<pair<int,int>>> signals(n);

    for (int i = 0; i < n; i++) {
        for (const auto &edge : signalsTemp[i]) {
            signals[i].push_back(make_pair(edge.first, edge.second));
        }
    }
    

    /* compute the uncertainty intervals */
    vector<vector<vector<int>>> uncertainties(n);

    for (int i = 0; i < n; i++) {
        int gi = signals[i].size();
        uncertainties[i].resize(gi);

        uncertainties[i][0].push_back(-del);
        for (int j = 1; j < gi; j++) {
            int tj = signals[i][j].first;
            int low = max(uncertainties[i][j - 1][0] + del, tj - eps);
            uncertainties[i][j].push_back(low);
        }

        uncertainties[i][gi - 1].push_back(signals[i][gi - 1].first + eps);
        for (int j = gi - 2; j > 0; j--) {
            int tj = signals[i][j].first;
            int high = min(uncertainties[i][j + 1][1] - del, tj + eps);
            uncertainties[i][j].push_back(high);
        }
    }

    /* compute the canonical segmentation */
    set<int> segmentation_temp;

    for (const auto &s : uncertainties) {
        for (int j = 1; j < s.size(); j++) {
            for (auto b : s[j]) {
                segmentation_temp.insert(b);
            }
        }
    }

    vector<int> segmentation;
    for (const auto &b : segmentation_temp) {
        segmentation.push_back(b);
    }

    int numSegments = segmentation.size() - 1;

    /* compute the value expressions TODO: check and improve*/
    vector<vector<set<string>>> valExprs(n);
    
    for (int i = 0; i < n; i++) {
        valExprs[i].resize(numSegments);

        for (int j = 0; j < numSegments; j++) {
            valExprs[i][j].insert("");

            for (int k = 1; k < uncertainties[i].size(); k++) {
                if (segmentation[j] == uncertainties[i][k][0] && segmentation[j + 1] == uncertainties[i][k][1]) {
                    // entire expression
                    set<string> temp;
                    string expr = to_string(signals[i][k - 1].second) + ";" + to_string(signals[i][k].second);
                    temp.insert("");
                    temp.insert(expr);
                    valExprs[i][j] = concatWithDestutter(valExprs[i][j], temp);
                }
                else if (segmentation[j] == uncertainties[i][k][0] && segmentation[j + 1] < uncertainties[i][k][1]) {
                    // prefix
                    set<string> temp;
                    string expr = to_string(signals[i][k - 1].second) + ";" + to_string(signals[i][k].second);
                    temp.insert("");
                    temp.insert(expr);
                    temp.insert(expr.substr(0, expr.find(';')));
                    valExprs[i][j] = concatWithDestutter(valExprs[i][j], temp);

                }
                else if (segmentation[j] > uncertainties[i][k][0] && segmentation[j + 1] == uncertainties[i][k][1]) {
                    // suffix
                    set<string> temp;
                    string expr = to_string(signals[i][k - 1].second) + ";" + to_string(signals[i][k].second);
                    temp.insert("");
                    temp.insert(expr);
                    temp.insert(expr.substr(expr.find(';') + 1, expr.length()));
                    valExprs[i][j] = concatWithDestutter(valExprs[i][j], temp);
                }
                else if (segmentation[j] > uncertainties[i][k][0] && segmentation[j + 1] < uncertainties[i][k][1]) {
                    // infix
                    set<string> temp;
                    string expr = to_string(signals[i][k - 1].second) + ";" + to_string(signals[i][k].second);
                    temp.insert("");
                    temp.insert(expr);
                    temp.insert(expr.substr(0, expr.find(';')));
                    temp.insert(expr.substr(expr.find(';') + 1, expr.length()));
                    valExprs[i][j] = concatWithDestutter(valExprs[i][j], temp);
                }
            }

            if (valExprs[i][j].size() == 1) {
                int kk = 1;
                while (kk < uncertainties[i].size() && uncertainties[i][kk][1] <= segmentation[j]) {
                    kk++;
                }
                valExprs[i][j].insert(to_string(signals[i][kk - 1].second));
            }
        }
    }



    /* translate string value expressions to bit vector representation */

    vector<vector<vector<bitset<SIZE>>>> aps;

    convertIntoBitset(aps, valExprs);

    int test;


    return 0;
}

