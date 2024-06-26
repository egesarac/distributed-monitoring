#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <random>
#include <tuple>
#include <map>
#include <set>
#include <set>
#include <bitset>
#include <algorithm>
#include <limits>
#include <cfloat>
#include <chrono>
using namespace std;

#define SIZE 100 // TODO: this should be as tight as possible

bitset<SIZE> evenMask;
bitset<SIZE> oddMask;

chrono::time_point<chrono::system_clock> starttime;
chrono::time_point<chrono::system_clock> endtime;


template<std::size_t N>
int msb(const bitset<N> &bs) {
    for (int i = bs.size() - 1; i >= 0; i--) {
        if (bs[i] == true) {
            return i;
        }
    }

    return -1;
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

template<std::size_t N>
vector<set<string>> bitset2stringset_withSegments(const vector<vector<bitset<N>>> &v) {
    vector<set<string>> out(v.size());

    for (int i = 0; i < v.size(); i++) {
        out[i] = bitset2stringset(v[i]);
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

vector<set<string>> prodAlways(const vector<set<string>> &product) {
    vector<set<string>> out(product.size());

    bool firstBit0 = false;
    bool firstBit1 = true;

    for (int i = product.size() - 1; i >= 0; i--) {
        if (firstBit0 == true) {
            out[i].insert("0");
        }
            
        if (firstBit1 == true) {
            bool f1 = false;
            bool f2 = false;
            bool f3 = false; 

            for (const auto &s : product[i]) {
                if (s == "1") {
                    out[i].insert("1");
                    f1 = true;
                }

                else if (s == "0" || s.substr(s.length() - 2, 2) == "10") {
                    out[i].insert("0");
                    f2 = true;
                }

                else {
                    out[i].insert("01");
                    f3 = true;
                }

                if (f1 && f2 && f3) {
                    break;
                }
            }
        }

        if (out[i].find("0") != out[i].end() || out[i].find("01") != out[i].end()) {
            firstBit0 = true;
        }
        else {
            firstBit0 = false;
        }

        if (out[i].find("1") != out[i].end()) {
            firstBit1 = true;
        }
        else {
            firstBit1 = false;
        }
    }

    return out;
}

vector<set<string>> prodEventually(const vector<set<string>> &product) {
    vector<set<string>> out(product.size());

    bool firstBit0 = true;
    bool firstBit1 = false;

    for (int i = product.size() - 1; i >= 0; i--) {

        if (firstBit0 == true) {
            bool f1 = false;
            bool f2 = false;
            bool f3 = false; 

            for (const auto &s : product[i]) {
                if (s == "0") {
                    out[i].insert("0");
                    f1 = true;
                }

                else if (s == "1" || s.substr(s.length() - 2, 2) == "01") {
                    out[i].insert("1");
                    f2 = true;
                }

                else {
                    out[i].insert("10");
                    f3 = true;
                }

                if (f1 && f2 && f3) {
                    break;
                }
            }
        }
            
        if (firstBit1 == true) {
            out[i].insert("1");
        }

        if (out[i].find("0") != out[i].end()) {
            firstBit0 = true;
        }
        else {
            firstBit0 = false;
        }

        if (out[i].find("1") != out[i].end() || out[i].find("10") != out[i].end()) {
            firstBit1 = true;
        }
        else {
            firstBit1 = false;
        }
    }

    return out;
}

vector<set<string>> prodUntil0(const vector<set<pair<string, string>>> &product) {
    vector<set<string>> out(product.size());

    vector<set<char>> firstBits(product.size() + 1);
    firstBits[product.size()].insert('0');


    for (int i = product.size() - 1; i >= 0; i--) {
        set<string> temp;

        for (const auto &p : product[i]) {
            int len = p.second.length();

            for (const auto b : firstBits[i + 1]) {
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

vector<set<string>> prodUntil1(const vector<set<pair<string, string>>> &product) {
    vector<set<string>> out(product.size());

    vector<set<char>> firstBits(product.size() + 1);
    firstBits[product.size()].insert('1');


    for (int i = product.size() - 1; i >= 0; i--) {
        set<string> temp;

        for (const auto &p : product[i]) {
            int len = p.second.length();

            for (const auto b : firstBits[i + 1]) {
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

vector<set<string>> prodConjunction(const vector<set<pair<string, string>>> &product) {
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

vector<set<string>> prodNegation(const vector<set<string>> &product) {
    vector<set<string>> out(product.size());

    for (int i = 0; i < product.size(); i++) {
        for (const auto &p : product[i]) {
            int len = p.length();
            string s = "";

            for (int j = 0; j < len; j++) {
                if (p[j] == '1') {
                    s += "0";
                }
                else {
                    s += "1";
                }
            }

            out[i].insert(s);
        }
    }

    return out;
}

template<std::size_t N>
bool isEqual (vector<set<string>> productResult, vector<vector<bitset<N>>> bitsetResult) {
    bool flag = true;

    for (int i = 0; i < productResult.size(); i++) {
        set<string> S = bitset2stringset(bitsetResult[i]);

        if (S != productResult[i]) {
            flag = false;
            cout << "Error in segment " << i << endl;
        }
    }

    return flag;
}

pair<int, int> getNextIntervalForProfiles(const vector<int> &segmentation, const int &t1, const int &t2, const bool &leftOpen, const bool &rightOpen) { //TODO: real valued time points
    int s1, s2;

    return make_pair(s1, s2);
}

template<std::size_t N>
vector<bitset<N>> bitsetConcat(const vector<bitset<N>> &v1, const vector<bitset<N>> &v2) {
    vector<bitset<N>> out(2);
    // TODO: check and fix -- v1 empty? 
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j <= msb(v1[i]); j++) {
            if (v1[i][j] == true) {
                for (int x = 0; x < 2; x++) {
                    for (int y = 0; y <= msb(v2[x]); y++) {
                        if (v2[x][y] == true) {
                            if ((i == x && j % 2 == 1) || (i != x && j % 2 == 0)) {
                                out[i][j + y + 1] = true;
                            }
                            else {
                                out[i][j + y] = true;
                            }
                        }
                    }
                }
            }
        }
    }

    return out;
}

template<std::size_t N>
vector<vector<bitset<N>>> getProfiles(vector<vector<bitset<N>>> v, vector<double> segmentation, const double &t1, const double &t2, const double &a, const double &b, const bool &leftClosed, const bool &rightClosed) { //TODO: real valued time points
    vector<vector<bitset<N>>> profiles;

    // to handle the cases where the window falls out of the signal
    segmentation.push_back(INT32_MAX);
    vector<bitset<N>> temp(2);
    temp[0][0] = true;
    v.push_back(temp);

    vector<vector<double>> breakpoints(2);


    // TODO: check and improve
    breakpoints[0].push_back(t1 + a);
    int low0 = upper_bound(segmentation.begin(), segmentation.end(), t1 + a) - segmentation.begin();
    int high0 = lower_bound(segmentation.begin(), segmentation.end(), t2 + a) - segmentation.begin() - 1;
    for (int i = low0; i <= high0; i++) {
        breakpoints[0].push_back(segmentation[i] - 0.000001);
        breakpoints[0].push_back(segmentation[i]);
    }
    if (segmentation[high0] < t2 + a) {
        breakpoints[0].push_back(t2 + a - 0.000001);
        breakpoints[0].push_back(t2 + a);
    }

    breakpoints[1].push_back(t1 + b);
    int low1 = upper_bound(segmentation.begin(), segmentation.end(), t1 + b) - segmentation.begin();
    int high1 = lower_bound(segmentation.begin(), segmentation.end(), t2 + b) - segmentation.begin() - 1;
    for (int i = low1; i <= high1; i++) {
        breakpoints[1].push_back(segmentation[i] - 0.000001);
        breakpoints[1].push_back(segmentation[i]);
    }
    if (segmentation[high1] < t2 + b) {
        breakpoints[1].push_back(t2 + b - 0.000001);
        breakpoints[1].push_back(t2 + b);
    }

    int l0 = breakpoints[0].size() - 1; // to ignore the last interval
    int l1 = breakpoints[1].size() - 1;

    int i0 = 0;
    int i1 = 0;

    while (i0 < l0 && i1 < l1) {
        // find the relation of the current window to the segments, determine which actions to carry for the profile (prefix, suffix, etc)
    

        // check if we can fit another profile before this one


        /*alternative to above: add artificial breakpoints to capture the check step
            while populating the breakpoints vector, add for each x first the value x-dbl_min then x
            first implement this, then check the other
        */

        double left = breakpoints[0][i0];       
        double right = breakpoints[1][i1];


        // functions here depend on rightClosed -- might have to concat the first bits of the next segment
        //int xind = low0 + ((i0 + 1) % 2);
        int xind = upper_bound(segmentation.begin() + low0 - 1, segmentation.begin() + high1, left) - segmentation.begin() - 1;
        //int yind = low1 + ((i1 + 1) % 2);
        int yind = lower_bound(segmentation.begin() + low0 - 1, segmentation.begin() + high1, right) - segmentation.begin();

        if (yind - xind == 0) { // the two ends of the window belong to the same segment
            if (left == segmentation[xind]) { // the window starts together with the segment
                /*concat: prefix*/
            }
            else { // the window starts after the beginning of the segment
                /*concat: infix*/
            }
        }
        else { // the two ends of the window fall in different segments
            if (left == segmentation[xind]) {
                /*concat: entire segment*/
            }
            else {
                /*concat: suffix*/

            }
    
            /*concat: every full segment between two end points*/

            // TODO: fix below
            if (right == segmentation[yind] && rightClosed) {
                /*concat: firstBit*/
            }
            else if (right != segmentation[yind]) {
                /*concat: prefix*/
            }
        }

        


        // slide the window
        if (breakpoints[0][i0 + 1] - breakpoints[0][i0] < breakpoints[1][i1 + 1] - breakpoints[1][i1]) {
            breakpoints[1][i1] += (breakpoints[0][i0 + 1] - breakpoints[0][i0]);
            i0++;
        }
        else if (breakpoints[0][i0 + 1] - breakpoints[0][i0] > breakpoints[1][i1 + 1] - breakpoints[1][i1]) {
            breakpoints[0][i0] += (breakpoints[1][i1 + 1] - breakpoints[1][i1]);
            i1++;

        }
        else {
            breakpoints[1][i1] += (breakpoints[0][i0 + 1] - breakpoints[0][i0]);
            i0++;
            i1++;
        }
    }

    return profiles;
}

template<std::size_t N>
vector<vector<bitset<N>>> bitsetInfix(vector<vector<bitset<N>>> v1) {
    for (int i = 0; i < v1.size(); i++) {
        int maxInd = max(msb(v1[i][0] & (evenMask | oddMask)), msb(v1[i][1] & (evenMask | oddMask)));

        for (int j = 0; j < maxInd; j++) {
            v1[i][0][j] = true;
            v1[i][1][j] = true;
        }
    }

    return v1;
}

template<std::size_t N>
vector<vector<bitset<N>>> bitsetSuffix(vector<vector<bitset<N>>> v1) {
    for (int i = 0; i < v1.size(); i++) {
        int maxInd0 = max(msb(v1[i][0] & evenMask), msb(v1[i][1] & oddMask));
        int maxInd1 = max(msb(v1[i][0] & oddMask), msb(v1[i][1] & evenMask));

        for (int j = maxInd0; j >= 0; j--) {
            if (j % 2 == 0) {
                v1[i][0][j] = true;
            }
            else {
                v1[i][1][j] = true;
            }
        }

        for (int j = maxInd1; j >= 0; j--) {
            if (j % 2 == 0) {
                v1[i][1][j] = true;
            }
            else {
                v1[i][0][j] = true;
            }
        }
    }

    return v1;
}

template<std::size_t N>
vector<vector<bitset<N>>> bitsetPrefix(vector<vector<bitset<N>>> v1) {
    for (int i = 0; i < v1.size(); i++) {
        int maxInd0 = msb(v1[i][0] & (evenMask | oddMask));
        int maxInd1 = msb(v1[i][1] & (evenMask | oddMask));

        for (int j = 0; j <= maxInd0; j++) {
            v1[i][0][j] = true;
        }

        for (int j = 0; j <= maxInd1; j++) {
            v1[i][1][j] = true;
        }
    }

    return v1;
}

template<std::size_t N>
vector<vector<bitset<N>>> bitsetAlways(const vector<vector<bitset<N>>> &v1) {
    vector<vector<bitset<N>>> vv(v1.size()); 

    for (auto & v : vv) {
        v.resize(2);
    }

    bool firstBit0 = false;
    bool firstBit1 = true;

    for (int i = v1.size() - 1; i >= 0; i--) {
        if (firstBit0 == true) {
            vv[i][0][0] = true;
        }

        if (firstBit1 == true) {
            vector<int> len1(4);
            len1[0] = msb(v1[i][0] & evenMask) + 1; // 0...0
            len1[1] = msb(v1[i][0] & oddMask) + 1; // 0...1
            len1[2] = msb(v1[i][1] & oddMask) + 1; // 1...0
            len1[3] = msb(v1[i][1] & evenMask) + 1; // 1...1
            
            vv[i][0][0] = (max(len1[0], len1[2]) > 0); 
            vv[i][0][1] = (max(len1[1], len1[3]-2) > 0);
            vv[i][1][0] = v1[i][1][0];
        }

        if ((vv[i][0][0] == true) || (vv[i][0][1] == true)) {
            firstBit0 = true;
        }
        else {
            firstBit0 = false;
        }

        if (vv[i][1][0] == true) {
            firstBit1 = true;
        }
        else {
            firstBit1 = false;
        }
    }

    return vv;
}

template<std::size_t N>
vector<vector<bitset<N>>> bitsetEventually(const vector<vector<bitset<N>>> &v1) {
    vector<vector<bitset<N>>> vv(v1.size()); 

    for (auto & v : vv) {
        v.resize(2);
    }

    bool firstBit0 = true;
    bool firstBit1 = false;

    for (int i = v1.size() - 1; i >= 0; i--) {
        if (firstBit0 == true) {
            vector<int> len1(4);
            len1[0] = msb(v1[i][0] & evenMask) + 1; // 0...0
            len1[1] = msb(v1[i][0] & oddMask) + 1; // 0...1
            len1[2] = msb(v1[i][1] & oddMask) + 1; // 1...0
            len1[3] = msb(v1[i][1] & evenMask) + 1; // 1...1
            
            vv[i][0][0] = v1[i][0][0];
            vv[i][1][0] = (max(len1[1], len1[3]) > 0);
            vv[i][1][1] = (max(len1[0]-2, len1[2]) > 0);
        }

        if (firstBit1 == true) {
            vv[i][1][0] = true;
        }

        if (vv[i][0][0] == true) {
            firstBit0 = true;
        }
        else {
            firstBit0 = false;
        }

        if ((vv[i][1][0] == true) || (vv[i][1][1] == true)) {
            firstBit1 = true;
        }
        else {
            firstBit1 = false;
        }
    }

    return vv;
}

template<std::size_t N>
vector<vector<bitset<N>>> bitsetUntil(vector<vector<bitset<N>>> v1, vector<vector<bitset<N>>> v2) {
    vector<vector<bitset<N>>> vv(v1.size()); 

    for (auto & v : vv) {
        v.resize(2);
    }

    bool firstBit0 = false;
    bool firstBit1 = true;

    for (int i = v1.size() - 1; i >= 0; i--) {
        // TODO: improve this
        vector<bool> save(4);
        save[0] = v1[i][0][0];
        save[1] = v1[i][1][0];
        save[2] = v2[i][0][0];
        save[3] = v2[i][1][0];


        if (firstBit0 == true) {
            // handling the corner cases of 0 U x, 1 U x, x U 0, x U 1
            if (v1[i][0][0] == true) {
                vv[i][0] = vv[i][0] | v2[i][0];
                vv[i][1] = vv[i][1] | v2[i][1];
                v1[i][0][0] = false;
            }

            if (v1[i][1][0] == true) {
                vector<int> temp(4);
                temp[0] = msb(v2[i][0] & evenMask) + 1; // 0...0
                temp[1] = msb(v2[i][0] & oddMask) + 1; // 0...1
                temp[2] = msb(v2[i][1] & oddMask) + 1; // 1...0
                temp[3] = msb(v2[i][1] & evenMask) + 1; // 1...1
                
                vv[i][0][0] = vv[i][0][0] || v2[i][0][0];
                vv[i][1][0] = vv[i][1][0] || (max(temp[1], temp[3]) > 0);
                vv[i][1][1] = vv[i][1][1] || (max(temp[0]-2, temp[2]) > 0);

                v1[i][1][0] = false;
            }

            if (v2[i][0][0] == true) {
                vv[i][0][0] = true;
                v2[i][0][0] = false;
            }

            if (v2[i][1][0] == true) {
                vv[i][1][0] = true;
                v2[i][1][0] = false;
            }

            if ((v1[i][0].none() && v1[i][1].none()) || (v2[i][0].none() && v2[i][1].none())) {
                continue;
            }

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

            vector<int> len_strong(4);
            len_strong[0] = len2[0];
            len_strong[1] = len2[1];
            len_strong[2] = (max(len1[2], len1[3]) > 0) ? max(len2[2], len2[0]-1) : len2[2];
            len_strong[3] = (max(len1[2], len1[3]) > 0) ? max(len2[3], len2[1]-1) : len2[3];

            int ctr;
            
            // 0...0
            ctr = len_strong[0] - 1;
            if (ctr > -1) {
                if (len1[2] == 2 && len1[0] == 0 && len1[1] == 0 && len1[3] == 0) {
                    while (ctr > 0) {
                        vv[i][0][ctr] = v2[i][0][ctr];
                        ctr -= 2;
                    }
                }
                else {
                    while (ctr > 0) {
                        vv[i][0][ctr] = true;
                        ctr -= 2;
                    }
                }
            }

            // 0...1
            ctr = len_strong[1] - 1;
            if (ctr > -1) {
                if (len1[2] == 2 && len1[0] == 0 && len1[1] == 0 && len1[3] == 0) {
                    while (ctr > 0) {
                        vv[i][0][ctr] = v2[i][0][ctr];
                        ctr -= 2;
                    }
                }
                else {
                    while (ctr > 0) {
                        vv[i][0][ctr] = true;
                        ctr -= 2;
                    }
                }
            }

            // 1...0
            ctr = len_strong[2] - 1;
            while (ctr > 0) { 
                vv[i][1][ctr] = true;
                ctr -= 2;
            }

            // 1...1
            ctr = len_strong[3] - 1;
            while (ctr >= 0) { 
                vv[i][1][ctr] = true;
                ctr -= 2;
            }
        }

        if (firstBit1 == true) {
            v1[i][0][0] = save[0];
            v1[i][1][0] = save[1];
            v2[i][0][0] = save[2];
            v2[i][1][0] = save[3];

            // handling the corner cases of 0Ux=x, 1Ux=1, xU0=Ax, xU1=1
            if (v2[i][0][0] == true) {
                vector<int> temp(4);
                temp[0] = msb(v1[i][0] & evenMask) + 1; // 0...0
                temp[1] = msb(v1[i][0] & oddMask) + 1; // 0...1
                temp[2] = msb(v1[i][1] & oddMask) + 1; // 1...0
                temp[3] = msb(v1[i][1] & evenMask) + 1; // 1...1
                
                vv[i][0][0] = vv[i][0][0] || (max(temp[0], temp[2]) > 0); 
                vv[i][0][1] = vv[i][0][1] || (max(temp[1], temp[3]-2) > 0);
                vv[i][1][0] = vv[i][1][0] || v1[i][1][0];

                v2[i][0][0] = false;
            }

            if (v1[i][0][0] == true) {
                v2[i][0][0] = save[2];

                vv[i][0] = vv[i][0] | v2[i][0];
                vv[i][1] = vv[i][1] | v2[i][1];
                v1[i][0][0] = false;

                v2[i][0][0] = false;
            }

            if (v1[i][1][0] == true) {
                vv[i][1][0] = true;
                v1[i][1][0] = false;
            }



            if (v2[i][1][0] == true) {
                vv[i][1][0] = true;
                v2[i][1][0] = false;
            }

            if ((v1[i][0].none() && v1[i][1].none()) || (v2[i][0].none() && v2[i][1].none())) {
                continue;
            }

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

            vector<int> len_weak(4);
            len_weak[0] = (max(len1[0], len1[2]) > 0) ? (len2[0]) : 0;
            len_weak[1] = ((max(len1[1], len1[3]) > 0) && len2[0] > 0) ? (max(len2[0]+1, len2[1])) : len2[1];

            if (len1[0] > 0 || len1[2] > 0) {
                if (len1[2] > 0 && len2[0] > 0) {
                    len_weak[2] = max(len2[2], len2[0]-1);
                }
                else {
                    len_weak[2] = len2[2];
                }
            }
            else {
                len_weak[2] = 0;
            }

            if (len1[3] > 0) {
                if (len2[2] > 0) {
                    len_weak[3] = max({len2[0], len2[1]-1, len2[2]+1, len2[3]});
                }
                else {
                    len_weak[3] = max({len2[0], len2[1]-1, len2[3]});
                }
            }
            else if (len1[2] > 0 && len1[1] > 0) {
                if (len2[2] > 0) {
                    len_weak[3] = max({len2[1]-1, len2[2]+1, len2[3]});
                }
                else {
                    len_weak[3] = max(len2[1]-1, len2[3]);
                }
            }
            else if (len1[2] > 0) {
                len_weak[3] = max(len2[1]-1, len2[3]);
            }
            else if (len1[1] > 0) {
                if (len2[2] > 0) {
                    len_weak[3] = max(len2[2]+1, len2[3]);
                }
                else {
                    len_weak[3] = len2[3];
                }
            }
            else {
                len_weak[3] = len2[3];
            }
            
            int ctr;

            // 0...0
            ctr = len_weak[0] - 1;
            if (ctr > -1) {
                if (len1[2] == 2 && len1[0] == 0) {
                    while (ctr > 0) {
                        vv[i][0][ctr] = v2[i][0][ctr];
                        ctr -= 2;
                    }
                }
                else {
                    while (ctr > 0) {
                        vv[i][0][ctr] = true;
                        ctr -= 2;
                    }
                }
            }

            // 0...1
            ctr = len_weak[1] - 1;
            if (ctr > -1) {
                if (len1[2] == 2 && len1[0] == 0 && len1[1] == 0 && len1[3] == 0) {
                   while (ctr > 0) {
                        vv[i][0][ctr] = v2[i][0][ctr];
                        ctr -= 2;
                    }
                }
                else {
                    while (ctr > 0) {
                        vv[i][0][ctr] = true;
                        ctr -= 2;
                    }
                }
            }

            // 1...0
            ctr = len_weak[2] - 1;
            if (ctr > -1) {
                if (len1[2] == 2 && len1[0] == 0 && len2[2] == 0) {
                    while (ctr > 0) {
                        vv[i][1][ctr] = v2[i][1][ctr];
                        ctr -= 2;
                    }
                    int cc = len2[0] - 2;
                    while (cc > 0) {
                        vv[i][1][cc] = true;
                        cc -= 2;
                    }
                }
                else {
                    while (ctr > 0) {
                        vv[i][1][ctr] = true;
                        ctr -= 2;
                    }
                }
            }

            // 1...1
            ctr = len_weak[3] - 1;
            while (ctr >= 0) { 
                vv[i][1][ctr] = true;
                ctr -= 2;
            }
        }
    }

    return vv;
}

template<std::size_t N>
vector<vector<bitset<N>>> bitsetConjunction(vector<vector<bitset<N>>> v1, vector<vector<bitset<N>>> v2) {
    vector<vector<bitset<N>>> vv(v1.size());

    for (auto & v : vv) {
        v.resize(2);
    }

    for (int i = 0; i < v1.size(); i++) {
        // handling the corner cases of conjunction with 0 or 1
        if (v1[i][0][0] == true) {
            vv[i][0][0] = true;
            v1[i][0][0] = false;
        }
        if (v2[i][0][0] == true) {
            vv[i][0][0] = true;
            v2[i][0][0] = false;
        }
        if (v1[i][1][0] == true) {
            vv[i][0] = vv[i][0] | v2[i][0];
            vv[i][1] = vv[i][1] | v2[i][1];
            v1[i][1][0] = false;
        }
        if (v2[i][1][0] == true) {
            vv[i][0] = vv[i][0] | v1[i][0];
            vv[i][1] = vv[i][1] | v1[i][1];
            v2[i][1][0] = false;
        }

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
                len1[i] = INT32_MIN / 2 + 2;
            }

            if (len2[i] == 0) {
                len2[i] = INT32_MIN / 2 + 2;
            }
        }

        vector<int> len(4);
        len[0] = max({len1[0]+len2[0]-2, len1[0]+len2[1]-1, len1[0]+len2[2]-1, len1[0]+len2[3], len1[1]+len2[0]-1, len1[1]+len2[2], len1[2]+len2[0]-1, len1[2]+len2[1], len1[3]+len2[0]}) - 1;
        len[1] = max({len1[1]+len2[1]-1, len1[1]+len2[3], len1[3]+len2[1]}) - 1;
        len[2] = max({len1[2]+len2[2]-1, len1[2]+len2[3], len1[3]+len2[2]}) - 1;
        len[3] = len1[3]+len2[3]-1;
        
        int ctr;
        for (int j = 0; j < 3; j++) {
            ctr = len[j] - 1;
    
            while (ctr >= 0) {
                vv[i][j / 2][ctr] = true;
                ctr = ctr - 2;
            }
        }
        ctr = len[3] - 1;
        while (ctr > 0) {
            vv[i][1][ctr] = true;
            ctr = ctr - 2;
        }
    }

    return vv;
}

template<std::size_t N>
vector<vector<bitset<N>>> bitsetNegation(vector<vector<bitset<N>>> v) {
    for (auto &vv : v) {
        std::reverse(vv.begin(), vv.end()); // swaps the bitvector0 and bitvector1 for each segment
    }

    return v;
}

// this only works for boolean signals
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

pair<int, int> convertIntoBoolWithDestutter(string str, string delimiter) {
    vector<bool> v;

    if (!str.empty()) {
        int start = 0;
        do {
            // Find the index of occurrence
            int idx = str.find(delimiter, start);
            if (idx == string::npos) {
                break;
            }
 

            int length = idx - start;
            if (stof(str.substr(start, length)) > 0) {
                if (v.empty() || v[v.size() - 1] != true){
                    v.push_back(true);
                }
            }
            else {
                if (v.empty() || v[v.size() - 1] != false){
                    v.push_back(false);
                }
            }
            start += (length + delimiter.size());
        } while (true);

        if (stof(str.substr(start, str.length())) > 0) {
            if (v.empty() || v[v.size() - 1] != true){
                v.push_back(true);
            }
        }
        else {
            if (v.empty() || v[v.size() - 1] != false){
                v.push_back(false);
            }
        }
    }

    int x = -1;
    int y = -1;

    if (!v.empty()) {
        if (v[0] == true) {
            x = 1;
        }
        else {
            x = 0;
        }

        y = v.size() - 1;
    }
 
    return make_pair(x, y);
}



int main() {

    vector<int> N {1, 2};
    vector<int> D {4, 8, 16};
    vector<double> EPS {1, 2, 4};
    vector<double> DEL {1, 2, 4};

    //int n = 1;
    int n = 2;
    
    ofstream results;
    string filename = "dummy_fp.txt";
    results.open(filename);

    for (const auto &d : D) {
        for (const auto &eps : EPS) {
            for (const auto &del : DEL) {
                vector<int> NUMEDGES;
                int num = 1; // FIX
                while (num <= ceil((d + eps) / del) + 1) {
                    NUMEDGES.push_back(num);
                    num = num * 2;
                }
                    
                for (const auto &numEdges1 : NUMEDGES) {
                    vector<vector<pair<double,double>>> signals(n);
                    string filename1 = "data/" + to_string(d) + "_" + to_string(int(eps)) + "_" + to_string(int(del)) + "_" + to_string(numEdges1) + "_1.txt";
                    ifstream sigdata1(filename1);

                    string line1;
                    while(getline(sigdata1, line1)) {
                        stringstream linestream(line1);
                        double t, v;
                        linestream >> t >> v;
                        signals[0].push_back(make_pair(t, v));
                    }
                    sigdata1.close();

                    if(signals[0].empty()) {
                        continue;
                    }

                    for (const auto &numEdges2 : NUMEDGES) {
                        signals[1].clear();
                        string filename2 = "data/" + to_string(d) + "_" + to_string(int(eps)) + "_" + to_string(int(del)) + "_" + to_string(numEdges2) + "_2.txt";
                        ifstream sigdata2(filename2);

                        string line2;
                        while(getline(sigdata2, line2)) {
                            stringstream linestream(line2);
                            double t, v;
                            linestream >> t >> v;
                            signals[1].push_back(make_pair(t, v));
                        }
                        sigdata2.close();

                        if(signals[1].empty()) {
                            continue;
                        }

                        

                        /* compute the uncertainty intervals */
                        starttime = chrono::system_clock::now();

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
                            }
                        }

                        endtime = chrono::system_clock::now();
                        chrono::duration<double, milli> intervalsTime = endtime - starttime;

                        /* compute the actual concurrent events */
                        // TODO

                        /* generate all valid interleavings */
                        // TODO

                        /* evaluate all valid interleavings and store in bitsets */
                        // TODO

                        /* compute the canonical segmentation */
                        starttime = chrono::system_clock::now();

                        set<double> segmentation_temp;

                        segmentation_temp.insert(0.0);
                        segmentation_temp.insert(max(double(d), signals[0][signals[0].size() - 1].first));

                        for (const auto &s : uncertainties) {
                            for (int j = 1; j < s.size(); j++) {
                                for (auto b : s[j]) {
                                    segmentation_temp.insert(b);
                                }
                            }
                        }

                        vector<double> segmentation;
                        for (const auto &b : segmentation_temp) {
                            segmentation.push_back(b);
                        }

                        int numSegments = segmentation.size() - 1;

                        endtime = chrono::system_clock::now();
                        chrono::duration<double, milli> segmentationTime = endtime - starttime;

                        /* compute the value expressions */
                        // TODO: check and improve
                        starttime = chrono::system_clock::now();

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

                        endtime = chrono::system_clock::now();
                        chrono::duration<double, milli> valExprsTime = endtime - starttime;

                        /* translate real-valued signals to atomic propositions */
                        starttime = chrono::system_clock::now();

                        vector<vector<vector<bitset<SIZE>>>> aps(n);
                        for (auto &v : aps) {
                            v.resize(numSegments);
                            for (auto &vv : v) {
                                vv.resize(2);
                            }
                        }

                        for (int i = 0; i < n; i++) {
                            for (int j = 0; j < numSegments; j++) {
                                for (auto &expr : valExprs[i][j]) {
                                    if (expr != "") {
                                        pair<int, int> xy = convertIntoBoolWithDestutter(expr, ";");
                                        if (xy.first >= 0 && xy.second >= 0) {
                                            aps[i][j][xy.first][xy.second] = true;
                                        }
                                    }
                                    
                                }
                            }
                        }

                        endtime = chrono::system_clock::now();
                        chrono::duration<double, milli> apsTime = endtime - starttime;

                        for (int i = 0; i < SIZE; i++) {
                            if (i % 2 == 0) {
                                evenMask[i] = true;
                            }
                            else {
                                oddMask[i] = true;
                            }
                        }
                        
                        
                    
                        starttime = chrono::system_clock::now();
                        vector<vector<bitset<SIZE>>> test = bitsetUntil(aps[0], aps[1]);
                        //vector<vector<bitset<SIZE>>> test = bitsetEventually(aps[0]);
                        endtime = chrono::system_clock::now();
                        chrono::duration<double, milli> evalTime = endtime - starttime;
                        //cout << evalTime.count() << endl;

                        //results << d << " " << eps << " " << del << " " << numEdges1 << " " << numSegments << " " << intervalsTime.count() << " " << segmentationTime.count() << " " << valExprsTime.count() << " " << apsTime.count() << " " << evalTime.count() << endl;                        
                        results << d << " " << eps << " " << del << " " << numEdges1 << " " << numEdges2 << " " << numSegments << " " << intervalsTime.count() << " " << segmentationTime.count() << " " << valExprsTime.count() << " " << apsTime.count() << " " << evalTime.count() << endl;                        
                        //cout << d << " " << eps << " " << del << " " << numEdges1 << " " << numSegments << " " << intervalsTime.count() << " " << segmentationTime.count() << " " << valExprsTime.count() << " " << apsTime.count() << " " << evalTime.count() << endl;                              
                        cout << d << " " << eps << " " << del << " " << numEdges1 << " " << numEdges2 << " " << numSegments << " " << intervalsTime.count() << " " << segmentationTime.count() << " " << valExprsTime.count() << " " << apsTime.count() << " " << evalTime.count() << endl;                              
                    }
                }
            }
        }
    }

    results.close();

    return 0;
}

