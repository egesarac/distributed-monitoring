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
using namespace std;

#define SIZE 100

bitset<SIZE> evenMask;
bitset<SIZE> oddMask;

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
vector<vector<bitset<N>>> getProfiles(vector<vector<bitset<N>>> v, vector<int> segmentation, const double &t1, const double &t2, const double &a, const double &b, const bool &leftOpen, const bool &rightOpen) { //TODO: real valued time points
    vector<vector<bitset<N>>> profiles;

    // to handle the cases where the window falls out of the signal
    segmentation.push_back(INT32_MAX);
    vector<bitset<N>> temp(2);
    temp[0][0] = true;
    v.push_back(temp);

    vector<vector<int>> breakpoints(2);

    breakpoints[0].push_back(t1 + a);
    int low0 = upper_bound(segmentation.begin(), segmentation.end(), t1 + a) - segmentation.begin();
    int high0 = lower_bound(segmentation.begin(), segmentation.end(), t2 + a) - segmentation.begin();
    for (int i = low0; i <= high0; i++) {
        breakpoints[0].push_back(segmentation[i]);
    }
    if (segmentation[high0] < t2 + a) {
        breakpoints[0].push_back(t2 + a);
    }

    breakpoints[1].push_back(t1 + b);
    int low1 = upper_bound(segmentation.begin(), segmentation.end(), t1 + b) - segmentation.begin();
    int high1 = lower_bound(segmentation.begin(), segmentation.end(), t2 + b) - segmentation.begin();
    for (int i = low1; i <= high1; i++) {
        breakpoints[1].push_back(segmentation[i]);
    }
    if (segmentation[high1] < t2 + b) {
        breakpoints[1].push_back(t2 + b);
    }

    int l0 = breakpoints[0].size();
    int l1 = breakpoints[1].size();

    int i0 = 0;
    int i1 = 0;

    while (i0 < l0 && i1 < l1) {
        // find the relation of the current window to the segments, determine which actions to carry for the profile (prefix, suffix, etc)


        // check if we can fit another profile before this one


        /*alternative to above: add artificial breakpoints to capture the check step
            while populating the breakpoints vector, add for each x first the value x-dbl_min then x
            first implement this, then check the other
        */


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







    /*
    while (flag) {
        do something

        tie(x, y) = getNextIntervalForProfiles(...);
    }
    */

    int x = t1 + a;
    int y = min(t2 + b, segmentation[segmentation.size() - 1]);
    int low = lower_bound(segmentation.begin(), segmentation.end(), x) - segmentation.begin();
    int high = upper_bound(segmentation.begin(), segmentation.end(), y) - segmentation.begin();
    /*if (segmentation[high - 1] == y) {
        high--;
    }*/

    for (int i = low; i < high; i++) {
        //if (lower_bound(segmentation.begin(), segmentation.end(), x))
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
vector<vector<bitset<N>>> bitsetUntil(const vector<vector<bitset<N>>> &vv1, const vector<vector<bitset<N>>> &vv2) {
    vector<vector<bitset<N>>> vv(vv1.size()); 

    for (auto & v : vv) {
        v.resize(2);
    }

    bool firstBit0 = false;
    bool firstBit1 = true;

    for (int i = vv1.size() - 1; i >= 0; i--) {
        if (firstBit0 == true) {
            vector<vector<bitset<N>>> v1 = vv1;
            vector<vector<bitset<N>>> v2 = vv2;

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
                        vv[i][0][ctr] = vv2[i][0][ctr];
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
                        vv[i][0][ctr] = vv2[i][0][ctr];
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
            vector<vector<bitset<N>>> v1 = vv1;
            vector<vector<bitset<N>>> v2 = vv2;

            // handling the corner cases of 0Ux=x, 1Ux=1, xU0=Ax, xU1=1
            if (v1[i][0][0] == true) {
                vv[i][0] = vv[i][0] | vv2[i][0];
                vv[i][1] = vv[i][1] | vv2[i][1];
                v1[i][0][0] = false;
            }

            if (v1[i][1][0] == true) {
                vv[i][1][0] = true;
                v1[i][1][0] = false;
            }

            if (v2[i][0][0] == true) {
                vector<int> temp(4);
                temp[0] = msb(vv1[i][0] & evenMask) + 1; // 0...0
                temp[1] = msb(vv1[i][0] & oddMask) + 1; // 0...1
                temp[2] = msb(vv1[i][1] & oddMask) + 1; // 1...0
                temp[3] = msb(vv1[i][1] & evenMask) + 1; // 1...1
                
                vv[i][0][0] = vv[i][0][0] || (max(temp[0], temp[2]) > 0); 
                vv[i][0][1] = vv[i][0][1] || (max(temp[1], temp[3]-2) > 0);
                vv[i][1][0] = vv[i][1][0] || vv1[i][1][0];

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
                        vv[i][0][ctr] = vv2[i][0][ctr];
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
                        vv[i][0][ctr] = vv2[i][0][ctr];
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
                        vv[i][1][ctr] = vv2[i][1][ctr];
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
vector<vector<bitset<N>>> bitsetConjunction(const vector<vector<bitset<N>>> &vv1, const vector<vector<bitset<N>>> &vv2) {
    vector<vector<bitset<N>>> vv(vv1.size());

    for (auto & v : vv) {
        v.resize(2);
    }

    for (int i = 0; i < vv1.size(); i++) {
        vector<vector<bitset<N>>> v1 = vv1;
        vector<vector<bitset<N>>> v2 = vv2;
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

    vector<vector<vector<bitset<SIZE>>>> aps(2);
    for (auto &v : aps) {
        v.resize(1);
        v[0].resize(2);
    }

    /*
    // RANDOM INPUT
    uniform_int_distribution<> rrr(0, 32);
    //random_device rd;  // a seed source for the random number engine
    //mt19937 gn(rd()); // mersenne_twister_engine seeded with rd()
    for (int i = 0; i < 1000; i++) { //486 bad input
        int sd = i;
        cout << "seed " << sd << endl;
        mt19937 gn(sd);
        for (int j = 0; j < 4; j++) {
            aps[j/2][0][j%2] = rrr(gn);
        }
        
        vector<vector<bitset<SIZE>>> testBit = bitsetUntil(aps[0], aps[1]);

        vector<set<pair<string, string>>> pr = asyncProd(aps[0], aps[1]);
        //cout << i << " " << pr[0].size() << endl;
        vector<set<string>> testProd = prodUntil1(pr);

        if (!isEqual(testProd, testBit)) {
            cout << "check: " << i << endl;
        }    
    }
    */


    // EXHAUSTIVE INPUT
    int numBits = 8;
    int numBitsTwice = 2 * numBits;
    unsigned long long bound = (1 << numBitsTwice);

    unsigned int mask0 = (1 << numBits) - 1;
    unsigned int mask1 = ~mask0;

    for (int i = 1; i < bound; i++) {
        aps[0][0][0] = bitset<SIZE>(i & mask0);
        aps[0][0][1] = bitset<SIZE>((i & mask1) >> numBits);

        for (int j = 1; j < bound; j++) {
            aps[1][0][0] = bitset<SIZE>(j & mask0);
            aps[1][0][1] = bitset<SIZE>((j & mask1) >> numBits);

            vector<vector<bitset<SIZE>>> testBit = bitsetUntil(aps[0], aps[1]);
            vector<set<pair<string, string>>> pr = asyncProd(aps[0], aps[1]);
            vector<set<string>> testProd = prodUntil1(pr);

            if (!isEqual(testProd, testBit)) {
                cout << "check: " << i << " " << j << endl;
            }
        }

        /*vector<vector<bitset<SIZE>>> testBit = bitsetNegation(aps[0]);
        vector<set<string>> testProd = prodNegation(bitset2stringset_withSegments(aps[0]));
        if (!isEqual(testProd, testBit)) {
            cout << "check: " << i << endl;
        }*/
        
        /*vector<vector<bitset<SIZE>>> test1 = bitsetPrefix(aps[0]);
        vector<vector<bitset<SIZE>>> test2 = bitsetSuffix(aps[0]);
        vector<vector<bitset<SIZE>>> test3 = bitsetInfix(aps[0]);*/
    }
    
    int x = 0;
    
    /*
    // CONTROLLED INPUT
    aps[0][0][0] = bitset<SIZE>("");
    aps[0][0][1] = bitset<SIZE>("10");
    aps[1][0][0] = bitset<SIZE>("");
    aps[1][0][1] = bitset<SIZE>("100000");

    //vector<vector<bitset<SIZE>>> testBit = bitsetUntil(aps[0], aps[1]);
    vector<set<pair<string, string>>> pr = asyncProd(aps[0], aps[1]);
    //cout << i << " " << pr[0].size() << endl;
    vector<set<string>> testProd = prodUntil1(pr);

    if (!isEqual(testProd, testBit)) {
        cout << "check" << endl;
    }
    int asdf = 0;*/


    /*
    // EXHAUSTIVE, CONTROLLED, 1-1 INPUT
    int NN = 8;
    for (int a = 0; a < 4; a++) {
        for (int b = 0; b < 4; b++) {
            int M0, M1;

            if (a == 0 || a == 3) {
                M0 = NN - 1;
            }
            else {
                M0 = NN - 2;
            }

            if (b == 0 || b == 3) {
                M1 = NN - 1;
            }
            else {
                M1 = NN - 2;
            }

            
            for (int i = M0; i >= 0; i -= 2) {
                string s0(NN, '0');
                s0[i] = '1';

                for (int j = M1; j >= 0; j -= 2) {
                    string s1(NN, '0');
                    s1[j] = '1';

                    if (a < 2) {
                        aps[0][0][0] = bitset<SIZE>(s0);
                        aps[0][0][1] = bitset<SIZE>("0");
                    }
                    else {
                        aps[0][0][0] = bitset<SIZE>("0");
                        aps[0][0][1] = bitset<SIZE>(s0);
                    }

                    if (b < 2) {
                        aps[1][0][0] = bitset<SIZE>(s1);
                        aps[1][0][1] = bitset<SIZE>("0");
                    }
                    else {
                        aps[1][0][0] = bitset<SIZE>("0");
                        aps[1][0][1] = bitset<SIZE>(s1);
                    }

                    vector<vector<bitset<SIZE>>> testBit = bitsetUntil(aps[0], aps[1]);
                    vector<set<pair<string, string>>> pr = asyncProd(aps[0], aps[1]);
                    vector<set<string>> testProd = prodUntil0(pr);

                    if (!isEqual(testProd, testBit)) {
                        cout << "check: " << i  << " " << j << endl;
                    }             
                }
            }
        }
    }
    */







    /*
    // UNTIL
    vector<vector<vector<bitset<SIZE>>>> aps = convertIntoBitset<SIZE>(valExprs);
    vector<vector<bitset<SIZE>>> testBitUnt = bitsetUntil(aps[0], aps[1]);
    vector<set<string>> testProdUnt = prodUntil(pr);
    bool resUnt = isEqual(testProdUnt, testBitUnt);
    */

    /*
    // CONJUNCTION
    vector<vector<vector<bitset<SIZE>>>> aps = convertIntoBitset<SIZE>(valExprs);
    vector<vector<bitset<SIZE>>> testBitConj = bitsetConjunction(aps[0], aps[1]);
    vector<set<pair<string, string>>> pr = asyncProd(aps[0], aps[1]);
    vector<set<string>> testProdConj = prodConjunction(pr);
    bool resConj = isEqual(testProdConj, testBitConj);
    */

    /*
    // EVENTUALLY
    vector<vector<vector<bitset<SIZE>>>> aps = convertIntoBitset<SIZE>(valExprs);
    vector<vector<bitset<SIZE>>> testBitEv0 = bitsetEventually(aps[0]);
    vector<vector<bitset<SIZE>>> testBitEv1 = bitsetEventually(aps[1]);
    vector<set<string>> testProdEv0 = prodEventually(bitset2stringset_withSegments(aps[0]));
    vector<set<string>> testProdEv1 = prodEventually(bitset2stringset_withSegments(aps[1]));
    bool resEv0 = isEqual(testProdEv0, testBitEv0);
    bool resEv1 = isEqual(testProdEv1, testBitEv1);
    */

    /*
    // ALWAYS
    vector<vector<vector<bitset<SIZE>>>> aps = convertIntoBitset<SIZE>(valExprs);
    vector<vector<bitset<SIZE>>> testBitAl0 = bitsetAlways(aps[0]);
    vector<vector<bitset<SIZE>>> testBitAl1 = bitsetAlways(aps[1]);
    vector<set<string>> testProdAl0 = prodAlways(bitset2stringset_withSegments(aps[0]));
    vector<set<string>> testProdAl1 = prodAlways(bitset2stringset_withSegments(aps[1]));
    bool resAl0 = isEqual(testProdAl0, testBitAl0);
    bool resAl1 = isEqual(testProdAl1, testBitAl1);
    */

    return 0;
}

