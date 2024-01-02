#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <tuple>
#include <map>
#include <set>
#include <set>
#include <bitset>
#include <algorithm>
//#include <boost/dynamic_bitset.hpp>
using namespace std;
//using namespace boost;

#define SIZE 8

bitset<SIZE> evenMask;
bitset<SIZE> oddMask;

template<std::size_t N>
int msb(const std::bitset<N> &bs) {
  static_assert(N <= (CHAR_BIT * sizeof(unsigned long)), "bitset is too big");
  auto n = bs.to_ulong();
  // C++20 will have std::countl_zero() but in the meantime:
  //assert(n != 0); // clz is undefined if n is 0. Maybe return -1 in this case?
  if (n == 0) return -1;
  return (CHAR_BIT * sizeof n) - __builtin_clzl(n) - 1;
}

set<string> stutterOnce(const set<string> &S) {
    set<string> out;

    for (const auto &s : S) {
        for (int i = 0; i < s.length(); i++) {
            string sl = s.substr(0, i);
            string sr = s.substr(i, s.length());

            out.insert(sl + s[i] + sr);
        }
    } 

    return out;
}

set<string> stutter2k(const set<string> &S, const int &k) {
    int len = S.begin()->length();
    int iter = k - len;
    
    set<string> out = S;
    for (int i = 0; i < iter; i++) {
        set<string> temp = stutterOnce(out);
        out = temp;
    }

    return out;
}

template<std::size_t N>
set<string> bitset2stringset(const vector<bitset<N>> &v) {
    set<string> out;

    for (int i = 0; i < N; i++) {
        if (v[0][i] == true) {
            string s = "";

            for (int j = 0; j <= i; j++) {
                if (j % 2 == 0) {
                    s += "0";
                }
                else {
                    s += "1";
                }
            }

            out.insert(s);
        }

        if (v[1][i] == true) {
            string s = "";

            for (int j = 0; j <= i; j++) {
                if (j % 2 == 0) {
                    s += "1";
                }
                else {
                    s += "0";
                }
            }

            out.insert(s);
        }
    } 

    return out;
}

pair<string, string> destuttterPair(const string &t1, const string &t2) {
    string s1 = t1.substr(0,1);
    string s2 = t2.substr(0,1);
    int len = t1.length();

    for (int i = 1; i < len; i++) {
        if (t1[i - 1] != t1[i]  || t2[i - 1] != t2[i]) {
            s1 += t1[i];
            s2 += t2[i];
        }
    }

    return make_pair(s1, s2);
}

template<std::size_t N>
vector<set<pair<string, string>>> asyncProd(const vector<vector<bitset<N>>> &v1, const vector<vector<bitset<N>>> &v2) {
    vector<set<pair<string, string>>> out(v1.size());

    for (int i = 0; i < v1.size(); i++) {
        set<string> S1 = bitset2stringset(v1[i]);
        set<string> S2 = bitset2stringset(v2[i]);

        for (auto s1 : S1) {
            for (auto s2 : S2) {
                set<string> temp1, temp2;

                temp1.insert(s1);
                temp2.insert(s2);

                temp1 = stutter2k(temp1, s1.length() + s2.length() - 1);
                temp2 = stutter2k(temp2, s1.length() + s2.length() - 1);

                for (auto t1 : temp1) {
                    for (auto t2 : temp2) {
                        out[i].insert(destuttterPair(t1, t2));
                    }
                }
            }
        }
    }

    return out;
}

vector<set<string>> prodUntil(vector<set<pair<string, string>>> product) {
    vector<set<string>> out(product.size());

    vector<set<char>> firstBits(product.size() + 1);
    firstBits[product.size()].insert('0');


    for (int i = product.size() - 1; i >= 0; i--) {
        set<string> temp;

        for (const auto &p : product[i]) {
            int len = p.second.length();

            for (const auto b : firstBits[i + 1]) { // no need to compute twice, just change the last bit (from p.second if b=0, from p.first if b=1) 
                string s = "";
                s += b;

                for (int j = len - 1; j >= 0; j--) {
                    if (p.second[j] == '1' || (p.first[j] == '1' && s[len - 1 - j] == '1')) {
                        s += "1";
                    }
                    else {
                        s += "0";
                    }  
                }
                
                s = s.substr(1, s.length());
                string ss = s.substr(0,1);
                for (int j = 1; j < len; j++) {
                    if (s[j - 1] != s[j]) {
                        ss += s[j];
                    }
                }

                string sss(ss);
                std::reverse(sss.begin(), sss.end());

                temp.insert(sss);
            }
        }

        for (const auto &t : temp) {
            firstBits[i].insert(t[0]);
        }

        out[i] = temp;
    }

    return out;
}

vector<set<string>> prodConjunction(vector<set<pair<string, string>>> product) {
    vector<set<string>> out(product.size());

    for (int i = 0; i < product.size(); i++) {
        set<string> temp;

        for (const auto &p : product[i]) {
            int len = p.first.length();
            string s = "";

            for (int j = 0; j < len; j++) {
                if (p.first[j] == '1' && p.second[j] == '1') {
                    s += "1";
                }
                else {
                    s += "0";
                }
            }

            string ss = s.substr(0,1);
            for (int j = 1; j < len; j++) {
                if (s[j - 1] != s[j]) {
                    ss += s[j];
                }
            }

            temp.insert(ss);
        }

        out[i] = temp;
    }

    return out;
}

template<std::size_t N>
bool isEqual (vector<set<string>> productResult, vector<vector<bitset<N>>> bitsetResult) {
    for (int i = 0; i < productResult.size(); i++) {
        set<string> S = bitset2stringset(bitsetResult[i]);

        if (S != productResult[i]) {
            cout << i << endl;
        }
    }

    return true;
}

template<std::size_t N>
vector<vector<bitset<N>>> bitsetUntil(const vector<vector<bitset<N>>> &v1, const vector<vector<bitset<N>>> &v2) {
    vector<vector<bitset<N>>> vv(v1.size()); // TODO: fix 2, need to use the first bits of the next segment of the until

    for (auto & v : vv) {
        v.resize(2);
    }

    for (int i = 0; i < v1.size(); i++) {
        vector<int> len1(4);
        len1[0] = msb(v1[i][0] & evenMask) + 1; // 0...0
        len1[1] = msb(v1[i][0] & oddMask) + 1; // 0...1
        len1[2] = msb(v1[i][1] & oddMask) + 1; // 1...0
        len1[3] = msb(v1[i][1] & evenMask) + 1; // 1...1

        vector<int> len2(4);
        len2[0] = msb(v2[i][0] & evenMask) + 1; // 0...0
        len2[1] = msb(v2[i][0] & oddMask) + 1; // 0...1
        len2[2] = msb(v2[i][1] & oddMask) + 1; // 1...0
        len2[3] = msb(v2[i][1] & evenMask) + 1; // 1...1

        /*FIX BELOW*/

        // the special cases where v1 only has expressions of length 1
        if (len1[0] == 1 && len1[1] == 0 && len1[2] == 0 && len1[3] == 0) {
            vv[i] = v2[i];
            continue;
        }
        else if (len1[0] == 0 && len1[1] == 0 && len1[2] == 0 && len1[3] == 1) {
            if (v2[i][0][0] == true) {
                vv[i][0][0] = true;
            }

            if (max(len2[1], len2[3]) > 0) {
                vv[i][1][0] = true;
            }

            if (max(len2[0] - 1, len2[2]) > 0) {
                vv[i][1][1] = true;
            }

            continue;
        }
        else if (len1[0] == 1 && len1[1] == 0 && len1[2] == 0 && len1[3] == 1) {
            vv[i] = v2[i];

            if (max(len2[1], len2[3]) > 0) {
                vv[i][1][0] = true;
            }

            if (max(len2[0] - 1, len2[2]) > 0) {
                vv[i][1][1] = true;
            }

            continue;
        }
        
        vector<int> len(4);
        len[0] = len2[0];
        len[1] = len2[1];
        len[2] = (max(len1[2], len1[3]) > 0) ? max(len2[2], len2[0] - 1) : len2[2];
        len[3] = (max(len1[2], len1[3]) > 0) ? max(len2[3], len2[1] - 1) : len2[3];

        /*FIX ABOVE*/
        
        for (int j = 0; j < 4; j++) {
            int ctr = len[j] - 1;

            while (ctr >= 0) {
                vv[i][j / 2].set(ctr, true);
                ctr = ctr - 2;
            }
        }
    }

    return vv;
}

template<std::size_t N>
vector<vector<bitset<N>>> bitsetConjunction(const vector<vector<bitset<N>>> &v1, const vector<vector<bitset<N>>> &v2) {
    vector<vector<bitset<N>>> vv(v1.size());

    for (auto & v : vv) {
        v.resize(2);
    }

    for (int i = 0; i < v1.size(); i++) {
        vector<int> len1(4);
        len1[0] = msb(v1[i][0] & evenMask) + 1; // 0...0
        len1[1] = msb(v1[i][0] & oddMask) + 1; // 0...1
        len1[2] = msb(v1[i][1] & oddMask) + 1; // 1...0
        len1[3] = msb(v1[i][1] & evenMask) + 1; // 1...1

        vector<int> len2(4);
        len2[0] = msb(v2[i][0] & evenMask) + 1; // 0...0
        len2[1] = msb(v2[i][0] & oddMask) + 1; // 0...1
        len2[2] = msb(v2[i][1] & oddMask) + 1; // 1...0
        len2[3] = msb(v2[i][1] & evenMask) + 1; // 1...1

        for (int i = 0; i < 4; i++) {
            if (len1[i] == 0) {
                len1[i] = INT_MIN / 2 + 2;
            }

            if (len2[i] == 0) {
                len2[i] = INT_MIN / 2 + 2;
            }
        }

        vector<int> len(4);
        
        len[0] = max({len1[0]+len2[0], len1[0]+len2[1], len1[0]+len2[2], len1[0]+len2[3], len1[1]+len2[0], len1[1]+len2[2], len1[1]+len2[3], len1[2]+len2[0], len1[2]+len2[1], len1[3]+len2[0]}) - 1;
        len[1] = max({len1[1]+len2[1], len1[1]+len2[3], len1[3]+len2[1]}) - 1;
        len[2] = max({len1[2]+len2[2], len1[2]+len2[3], len1[3]+len2[2]}) - 1;
        len[3] = len1[3] + len2[3] - 1;

        // len[0] and len[3] have to be odd, the others even 
        if (len[0] >= 0 && len[0] % 2 == 0) {
            len[0]--;
        }
        if (len[1] >= 0 && len[1] % 2 == 1) {
            len[1]--;
        }
        if (len[2] >= 0 && len[2] % 2 == 1) {
            len[2]--;
        }
        if (len[3] >= 0 && len[3] % 2 == 0) {
            len[3]--;
        }
        
        for (int j = 0; j < 4; j++) {
            int ctr = len[j] - 1;

            while (ctr >= 0) {
                vv[i][j / 2].set(ctr, true);
                ctr = ctr - 2;
            }
        }

        if (v1[i][1][0] && v2[i][1][0]) {
            vv[i][1].set(0, true);
        }
        else {
            vv[i][1].set(0, false);
        }
    }

    return vv;
}

template<std::size_t N>
vector<bitset<N>> bitsetNegation(const vector<bitset<N>> &v) {
    vector<bitset<N>> vv(2);

    vv[0] = v[1];
    vv[1] = v[0];

    return vv;
}

template<std::size_t N>
vector<vector<vector<bitset<N>>>> convertIntoBitset(vector<vector<set<string>>> &valExprs) {
    vector<vector<vector<bitset<N>>>> aps(valExprs.size());

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

    return aps;
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
    int m;
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
    

    for (int i = 0; i < SIZE; i++) {
        if (i % 2 == 0) {
            evenMask.set(i, true);
        }
        else {
            oddMask.set(i, true);
        }
    }

    vector<vector<vector<bitset<SIZE>>>> aps = convertIntoBitset<SIZE>(valExprs);
    //vector<vector<bitset<SIZE>>> testBitConj = bitsetConjunction(aps[0], aps[1]);
    vector<vector<bitset<SIZE>>> testBitUnt = bitsetUntil(aps[0], aps[1]);

    vector<set<pair<string, string>>> pr = asyncProd(aps[0], aps[1]);
    //vector<set<string>> testProdConj = prodConjunction(pr);
    vector<set<string>> testProdUnt = prodUntil(pr);

    //bool resConj = isEqual(testProdConj, testBitConj);
    bool resUnt = isEqual(testProdUnt, testBitUnt);

    int ret0;

    return 0;
}

