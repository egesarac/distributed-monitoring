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

#define SIZE 1000 // TODO: this should be as tight as possible
#define REP 1

bitset<SIZE> evenMask;
bitset<SIZE> oddMask;
vector<vector<bitset<SIZE>>> test;

chrono::time_point<chrono::system_clock> starttime;
chrono::time_point<chrono::system_clock> endtime;

template <std::size_t N>
int msb(const bitset<N> &bs)
{
    for (int i = bs.size() - 1; i >= 0; i--)
    {
        if (bs[i] == true)
        {
            return i;
        }
    }

    return -1;
}

set<string> stutterOnce(const set<string> &S)
{
    set<string> out;

    for (const auto &s : S)
    {
        for (int i = 0; i < s.length(); i++)
        {
            string sl = s.substr(0, i);
            string sr = s.substr(i, s.length());

            out.insert(sl + s[i] + sr);
        }
    }

    return out;
}

set<string> stutter2k(const set<string> &S, const int &k)
{
    int len = S.begin()->length();
    int iter = k - len;

    set<string> out = S;
    for (int i = 0; i < iter; i++)
    {
        set<string> temp = stutterOnce(out);
        out = temp;
    }

    return out;
}

template <std::size_t N>
set<string> bitset2stringset(const vector<bitset<N>> &v)
{
    set<string> out;

    for (int i = 0; i < N; i++)
    {
        if (v[0][i] == true)
        {
            string s = "";

            for (int j = 0; j <= i; j++)
            {
                if (j % 2 == 0)
                {
                    s += "0";
                }
                else
                {
                    s += "1";
                }
            }

            out.insert(s);
        }

        if (v[1][i] == true)
        {
            string s = "";

            for (int j = 0; j <= i; j++)
            {
                if (j % 2 == 0)
                {
                    s += "1";
                }
                else
                {
                    s += "0";
                }
            }

            out.insert(s);
        }
    }

    return out;
}

template <std::size_t N>
vector<set<string>> bitset2stringset_withSegments(const vector<vector<bitset<N>>> &v)
{
    vector<set<string>> out(v.size());

    for (int i = 0; i < v.size(); i++)
    {
        out[i] = bitset2stringset(v[i]);
    }

    return out;
}

pair<string, string> destuttterPair(const string &t1, const string &t2)
{
    string s1 = t1.substr(0, 1);
    string s2 = t2.substr(0, 1);
    int len = t1.length();

    for (int i = 1; i < len; i++)
    {
        if (t1[i - 1] != t1[i] || t2[i - 1] != t2[i])
        {
            s1 += t1[i];
            s2 += t2[i];
        }
    }

    return make_pair(s1, s2);
}

template <std::size_t N>
vector<set<pair<string, string>>> asyncProd(const vector<vector<bitset<N>>> &v1, const vector<vector<bitset<N>>> &v2)
{
    vector<set<pair<string, string>>> out(v1.size());

    for (int i = 0; i < v1.size(); i++)
    {
        set<string> S1 = bitset2stringset(v1[i]);
        set<string> S2 = bitset2stringset(v2[i]);

        for (auto s1 : S1)
        {
            for (auto s2 : S2)
            {
                set<string> temp1, temp2;

                temp1.insert(s1);
                temp2.insert(s2);

                temp1 = stutter2k(temp1, s1.length() + s2.length() - 1);
                temp2 = stutter2k(temp2, s1.length() + s2.length() - 1);

                for (auto t1 : temp1)
                {
                    for (auto t2 : temp2)
                    {
                        out[i].insert(destuttterPair(t1, t2));
                    }
                }
            }
        }
    }

    return out;
}

vector<set<string>> prodAlways(const vector<set<string>> &product)
{
    vector<set<string>> out(product.size());

    bool firstBit0 = false;
    bool firstBit1 = true;

    for (int i = product.size() - 1; i >= 0; i--)
    {
        if (firstBit0 == true)
        {
            out[i].insert("0");
        }

        if (firstBit1 == true)
        {
            bool f1 = false;
            bool f2 = false;
            bool f3 = false;

            for (const auto &s : product[i])
            {
                if (s == "1")
                {
                    out[i].insert("1");
                    f1 = true;
                }

                else if (s == "0" || s.substr(s.length() - 2, 2) == "10")
                {
                    out[i].insert("0");
                    f2 = true;
                }

                else
                {
                    out[i].insert("01");
                    f3 = true;
                }

                if (f1 && f2 && f3)
                {
                    break;
                }
            }
        }

        if (out[i].find("0") != out[i].end() || out[i].find("01") != out[i].end())
        {
            firstBit0 = true;
        }
        else
        {
            firstBit0 = false;
        }

        if (out[i].find("1") != out[i].end())
        {
            firstBit1 = true;
        }
        else
        {
            firstBit1 = false;
        }
    }

    return out;
}

vector<set<string>> prodEventually(const vector<set<string>> &product)
{
    vector<set<string>> out(product.size());

    bool firstBit0 = true;
    bool firstBit1 = false;

    for (int i = product.size() - 1; i >= 0; i--)
    {

        if (firstBit0 == true)
        {
            bool f1 = false;
            bool f2 = false;
            bool f3 = false;

            for (const auto &s : product[i])
            {
                if (s == "0")
                {
                    out[i].insert("0");
                    f1 = true;
                }

                else if (s == "1" || s.substr(s.length() - 2, 2) == "01")
                {
                    out[i].insert("1");
                    f2 = true;
                }

                else
                {
                    out[i].insert("10");
                    f3 = true;
                }

                if (f1 && f2 && f3)
                {
                    break;
                }
            }
        }

        if (firstBit1 == true)
        {
            out[i].insert("1");
        }

        if (out[i].find("0") != out[i].end())
        {
            firstBit0 = true;
        }
        else
        {
            firstBit0 = false;
        }

        if (out[i].find("1") != out[i].end() || out[i].find("10") != out[i].end())
        {
            firstBit1 = true;
        }
        else
        {
            firstBit1 = false;
        }
    }

    return out;
}

vector<set<string>> prodUntil0(const vector<set<pair<string, string>>> &product)
{
    vector<set<string>> out(product.size());

    vector<set<char>> firstBits(product.size() + 1);
    firstBits[product.size()].insert('0');

    for (int i = product.size() - 1; i >= 0; i--)
    {
        set<string> temp;

        for (const auto &p : product[i])
        {
            int len = p.second.length();

            for (const auto b : firstBits[i + 1])
            {
                string s = "";
                s += b;

                for (int j = len - 1; j >= 0; j--)
                {
                    if (p.second[j] == '1' || (p.first[j] == '1' && s[len - 1 - j] == '1'))
                    {
                        s += "1";
                    }
                    else
                    {
                        s += "0";
                    }
                }

                s = s.substr(1, s.length());
                string ss = s.substr(0, 1);
                for (int j = 1; j < len; j++)
                {
                    if (s[j - 1] != s[j])
                    {
                        ss += s[j];
                    }
                }

                string sss(ss);
                std::reverse(sss.begin(), sss.end());

                temp.insert(sss);
            }
        }

        for (const auto &t : temp)
        {
            firstBits[i].insert(t[0]);
        }

        out[i] = temp;
    }

    return out;
}

vector<set<string>> prodUntil1(const vector<set<pair<string, string>>> &product)
{
    vector<set<string>> out(product.size());

    vector<set<char>> firstBits(product.size() + 1);
    firstBits[product.size()].insert('1');

    for (int i = product.size() - 1; i >= 0; i--)
    {
        set<string> temp;

        for (const auto &p : product[i])
        {
            int len = p.second.length();

            for (const auto b : firstBits[i + 1])
            {
                string s = "";
                s += b;

                for (int j = len - 1; j >= 0; j--)
                {
                    if (p.second[j] == '1' || (p.first[j] == '1' && s[len - 1 - j] == '1'))
                    {
                        s += "1";
                    }
                    else
                    {
                        s += "0";
                    }
                }

                s = s.substr(1, s.length());
                string ss = s.substr(0, 1);
                for (int j = 1; j < len; j++)
                {
                    if (s[j - 1] != s[j])
                    {
                        ss += s[j];
                    }
                }

                string sss(ss);
                std::reverse(sss.begin(), sss.end());

                temp.insert(sss);
            }
        }

        for (const auto &t : temp)
        {
            firstBits[i].insert(t[0]);
        }

        out[i] = temp;
    }

    return out;
}

vector<set<string>> prodConjunction(const vector<set<pair<string, string>>> &product)
{
    vector<set<string>> out(product.size());

    for (int i = 0; i < product.size(); i++)
    {
        set<string> temp;

        for (const auto &p : product[i])
        {
            int len = p.first.length();
            string s = "";

            for (int j = 0; j < len; j++)
            {
                if (p.first[j] == '1' && p.second[j] == '1')
                {
                    s += "1";
                }
                else
                {
                    s += "0";
                }
            }

            string ss = s.substr(0, 1);
            for (int j = 1; j < len; j++)
            {
                if (s[j - 1] != s[j])
                {
                    ss += s[j];
                }
            }

            temp.insert(ss);
        }

        out[i] = temp;
    }

    return out;
}

vector<set<string>> prodNegation(const vector<set<string>> &product)
{
    vector<set<string>> out(product.size());

    for (int i = 0; i < product.size(); i++)
    {
        for (const auto &p : product[i])
        {
            int len = p.length();
            string s = "";

            for (int j = 0; j < len; j++)
            {
                if (p[j] == '1')
                {
                    s += "0";
                }
                else
                {
                    s += "1";
                }
            }

            out[i].insert(s);
        }
    }

    return out;
}

template <std::size_t N>
bool isEqual(vector<set<string>> productResult, vector<vector<bitset<N>>> bitsetResult)
{
    bool flag = true;

    for (int i = 0; i < productResult.size(); i++)
    {
        set<string> S = bitset2stringset(bitsetResult[i]);

        if (S != productResult[i])
        {
            flag = false;
            cout << "Error in segment " << i << endl;
        }
    }

    return flag;
}

template <std::size_t N>
vector<bitset<N>> segmentFirstBit(vector<bitset<N>> v1)
{
    v1[0] = v1[0] & bitset<N>(1);
    v1[1] = v1[1] & bitset<N>(1);

    return v1;
}

template <std::size_t N>
vector<bitset<N>> segmentInfix(vector<bitset<N>> v1)
{
    int maxInd = max(msb(v1[0] & (evenMask | oddMask)), msb(v1[1] & (evenMask | oddMask)));

    for (int j = 0; j < maxInd; j++)
    {
        v1[0][j] = true;
        v1[1][j] = true;
    }

    return v1;
}

template <std::size_t N>
vector<bitset<N>> segmentSuffix(vector<bitset<N>> v1)
{
    int maxInd0 = max(msb(v1[0] & evenMask), msb(v1[1] & oddMask));
    int maxInd1 = max(msb(v1[0] & oddMask), msb(v1[1] & evenMask));

    for (int j = maxInd0; j >= 0; j--)
    {
        if (j % 2 == 0)
        {
            v1[0][j] = true;
        }
        else
        {
            v1[1][j] = true;
        }
    }

    for (int j = maxInd1; j >= 0; j--)
    {
        if (j % 2 == 0)
        {
            v1[1][j] = true;
        }
        else
        {
            v1[0][j] = true;
        }
    }

    return v1;
}

template <std::size_t N>
vector<bitset<N>> segmentPrefix(vector<bitset<N>> v1)
{
    int maxInd0 = msb(v1[0] & (evenMask | oddMask));
    int maxInd1 = msb(v1[1] & (evenMask | oddMask));

    for (int j = 0; j <= maxInd0; j++)
    {
        v1[0][j] = true;
    }

    for (int j = 0; j <= maxInd1; j++)
    {
        v1[1][j] = true;
    }

    return v1;
}

template <std::size_t N>
vector<vector<bitset<N>>> bitsetInfix(vector<vector<bitset<N>>> v1)
{
    for (int i = 0; i < v1.size(); i++)
    {
        int maxInd = max(msb(v1[i][0] & (evenMask | oddMask)), msb(v1[i][1] & (evenMask | oddMask)));

        for (int j = 0; j < maxInd; j++)
        {
            v1[i][0][j] = true;
            v1[i][1][j] = true;
        }
    }

    return v1;
}

template <std::size_t N>
vector<vector<bitset<N>>> bitsetSuffix(vector<vector<bitset<N>>> v1)
{
    for (int i = 0; i < v1.size(); i++)
    {
        int maxInd0 = max(msb(v1[i][0] & evenMask), msb(v1[i][1] & oddMask));
        int maxInd1 = max(msb(v1[i][0] & oddMask), msb(v1[i][1] & evenMask));

        for (int j = maxInd0; j >= 0; j--)
        {
            if (j % 2 == 0)
            {
                v1[i][0][j] = true;
            }
            else
            {
                v1[i][1][j] = true;
            }
        }

        for (int j = maxInd1; j >= 0; j--)
        {
            if (j % 2 == 0)
            {
                v1[i][1][j] = true;
            }
            else
            {
                v1[i][0][j] = true;
            }
        }
    }

    return v1;
}

template <std::size_t N>
vector<vector<bitset<N>>> bitsetPrefix(vector<vector<bitset<N>>> v1)
{
    for (int i = 0; i < v1.size(); i++)
    {
        int maxInd0 = msb(v1[i][0] & (evenMask | oddMask));
        int maxInd1 = msb(v1[i][1] & (evenMask | oddMask));

        for (int j = 0; j <= maxInd0; j++)
        {
            v1[i][0][j] = true;
        }

        for (int j = 0; j <= maxInd1; j++)
        {
            v1[i][1][j] = true;
        }
    }

    return v1;
}

template <std::size_t N>
vector<bitset<N>> bitsetConcat(const vector<bitset<N>> &v1, const vector<bitset<N>> &v2)
{
    if (v1[0].none() && v1[1].none())
    {
        return v2;
    }

    vector<bitset<N>> out(2);
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j <= msb(v1[i]); j++)
        {
            if (v1[i][j] == true)
            {
                for (int x = 0; x < 2; x++)
                {
                    for (int y = 0; y <= msb(v2[x]); y++)
                    {
                        if (v2[x][y] == true)
                        {
                            if ((i == x && j % 2 == 1) || (i != x && j % 2 == 0))
                            {
                                out[i][j + y + 1] = true;
                            }
                            else
                            {
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

template <std::size_t N>
vector<vector<bitset<N>>> getProfiles(vector<vector<bitset<N>>> v, vector<long long> segmentation, const long long &t1, const long long &t2, const long long &a, const long long &b, const bool &leftClosed, const bool &rightClosed)
{ // TODO: real valued time points
    vector<vector<bitset<N>>> profiles;

    // to handle the cases where the window falls out of the signal
    segmentation.push_back(INT32_MAX);
    vector<bitset<N>> temp(2);
    temp[0][0] = true;
    v.push_back(temp);

    vector<vector<long long>> breakpoints(2);

    // TODO: check and improve
    breakpoints[0].push_back(t1 + a);
    int low0 = upper_bound(segmentation.begin(), segmentation.end(), t1 + a) - segmentation.begin();
    int high0 = lower_bound(segmentation.begin(), segmentation.end(), t2 + a) - segmentation.begin() - 1;
    for (int i = low0; i <= high0; i++)
    {
        breakpoints[0].push_back(segmentation[i] - 1);
        breakpoints[0].push_back(segmentation[i]);
    }
    if (segmentation[high0] < t2 + a)
    {
        breakpoints[0].push_back(t2 + a - 1);
        breakpoints[0].push_back(t2 + a);
    }

    breakpoints[1].push_back(t1 + b);
    int low1 = upper_bound(segmentation.begin(), segmentation.end(), t1 + b) - segmentation.begin();
    int high1 = lower_bound(segmentation.begin(), segmentation.end(), t2 + b) - segmentation.begin() - 1;
    for (int i = low1; i <= high1; i++)
    {
        breakpoints[1].push_back(segmentation[i] - 1);
        breakpoints[1].push_back(segmentation[i]);
    }
    if (segmentation[high1] < t2 + b)
    {
        breakpoints[1].push_back(t2 + b - 1);
        breakpoints[1].push_back(t2 + b);
    }

    // to ignore the last interval which includes the point t2+b
    int l0 = breakpoints[0].size() - 1;
    int l1 = breakpoints[1].size() - 1;

    int i0 = 0;
    int i1 = 0;

    while (i0 < l0 && i1 < l1)
    {
        // find the relation of the current window to the segments, determine which actions to carry for the profile (prefix, suffix, etc)

        long long left = breakpoints[0][i0];
        long long right = breakpoints[1][i1];

        // TODO: check -> functions here depend on rightClosed -- might have to concat the first bits of the next segment
        // int xind = low0 + ((i0 + 1) % 2);
        int xind = upper_bound(segmentation.begin() + low0 - 1, segmentation.begin() + high1 + 1, left) - segmentation.begin() - 1;
        // int yind = low1 + ((i1 + 1) % 2);
        // int yind = lower_bound(segmentation.begin() + low0 - 1, segmentation.begin() + high1, right) - segmentation.begin();
        int yind = upper_bound(segmentation.begin() + low0 - 1, segmentation.begin() + high1 + 1, right) - segmentation.begin() - 1;

        vector<bitset<N>> pr(2);

        if (yind - xind == 0)
        { // the two ends of the window belong to the same segment
            if (left == segmentation[xind])
            { // the window starts together with the segment
                // concat: prefix
                pr = bitsetConcat(pr, segmentPrefix(v[xind]));
            }
            else
            { // the window starts after the beginning of the segment
                // concat: infix
                pr = bitsetConcat(pr, segmentInfix(v[xind]));
            }
        }
        else
        { // the two ends of the window fall in different segments
            if (left == segmentation[xind])
            {
                // concat: entire segment
                pr = bitsetConcat(pr, v[xind]);
            }
            else
            {
                // concat: suffix
                pr = bitsetConcat(pr, segmentSuffix(v[xind]));
            }

            // concat: every full segment between two end points
            for (int xx = xind + 1; xx < yind; xx++)
            {
                pr = bitsetConcat(pr, v[xx]);
            }

            // TODO: fix below
            if (right == segmentation[yind] && rightClosed)
            {
                // concat: firstBit
                pr = bitsetConcat(pr, segmentFirstBit(v[yind]));
            }
            else if (right != segmentation[yind])
            {
                // concat: prefix
                pr = bitsetConcat(pr, segmentPrefix(v[yind]));
            }
        }

        profiles.push_back(pr);

        // slide the window
        if (breakpoints[0][i0 + 1] - breakpoints[0][i0] < breakpoints[1][i1 + 1] - breakpoints[1][i1])
        {
            breakpoints[1][i1] += (breakpoints[0][i0 + 1] - breakpoints[0][i0]);
            i0++;
        }
        else if (breakpoints[0][i0 + 1] - breakpoints[0][i0] > breakpoints[1][i1 + 1] - breakpoints[1][i1])
        {
            breakpoints[0][i0] += (breakpoints[1][i1 + 1] - breakpoints[1][i1]);
            i1++;
        }
        else
        {
            breakpoints[1][i1] += (breakpoints[0][i0 + 1] - breakpoints[0][i0]);
            i0++;
            i1++;
        }
    }

    return profiles;
}

template <std::size_t N>
vector<vector<bitset<N>>> bitsetAlways(const vector<vector<bitset<N>>> &v1)
{
    vector<vector<bitset<N>>> vv(v1.size());

    for (auto &v : vv)
    {
        v.resize(2);
    }

    bool firstBit0 = false;
    bool firstBit1 = true;

    for (int i = v1.size() - 1; i >= 0; i--)
    {
        if (firstBit0 == true)
        {
            vv[i][0][0] = true;
        }

        if (firstBit1 == true)
        {
            vector<int> len1(4);
            len1[0] = msb(v1[i][0] & evenMask) + 1; // 0...0
            len1[1] = msb(v1[i][0] & oddMask) + 1;  // 0...1
            len1[2] = msb(v1[i][1] & oddMask) + 1;  // 1...0
            len1[3] = msb(v1[i][1] & evenMask) + 1; // 1...1

            vv[i][0][0] = (max(len1[0], len1[2]) > 0);
            vv[i][0][1] = (max(len1[1], len1[3] - 2) > 0);
            vv[i][1][0] = v1[i][1][0];
        }

        if ((vv[i][0][0] == true) || (vv[i][0][1] == true))
        {
            firstBit0 = true;
        }
        else
        {
            firstBit0 = false;
        }

        if (vv[i][1][0] == true)
        {
            firstBit1 = true;
        }
        else
        {
            firstBit1 = false;
        }
    }

    return vv;
}

template <std::size_t N>
vector<vector<bitset<N>>> bitsetEventually(const vector<vector<bitset<N>>> &v1)
{
    vector<vector<bitset<N>>> vv(v1.size());

    for (auto &v : vv)
    {
        v.resize(2);
    }

    bool firstBit0 = true;
    bool firstBit1 = false;

    for (int i = v1.size() - 1; i >= 0; i--)
    {
        if (firstBit0 == true)
        {
            vector<int> len1(4);
            len1[0] = msb(v1[i][0] & evenMask) + 1; // 0...0
            len1[1] = msb(v1[i][0] & oddMask) + 1;  // 0...1
            len1[2] = msb(v1[i][1] & oddMask) + 1;  // 1...0
            len1[3] = msb(v1[i][1] & evenMask) + 1; // 1...1

            vv[i][0][0] = v1[i][0][0];
            vv[i][1][0] = (max(len1[1], len1[3]) > 0);
            vv[i][1][1] = (max(len1[0] - 2, len1[2]) > 0);
        }

        if (firstBit1 == true)
        {
            vv[i][1][0] = true;
        }

        if (vv[i][0][0] == true)
        {
            firstBit0 = true;
        }
        else
        {
            firstBit0 = false;
        }

        if ((vv[i][1][0] == true) || (vv[i][1][1] == true))
        {
            firstBit1 = true;
        }
        else
        {
            firstBit1 = false;
        }
    }

    return vv;
}

template <std::size_t N>
vector<vector<bitset<N>>> bitsetBoundedAlways(vector<vector<bitset<N>>> &v1, vector<long long> segmentation, const long long &a, const long long &b, const bool &leftClosed, const bool &rightClosed)
{
    vector<vector<bitset<N>>> vv(v1.size());

    for (auto &v : vv)
    {
        v.resize(2);
    }

    for (int i = 0; i < v1.size(); i++)
    {
        vector<vector<bitset<N>>> temp = getProfiles(v1, segmentation, segmentation[i], segmentation[i + 1], a, b, leftClosed, rightClosed);

        temp = bitsetAlways(temp);
        for (int j = 1; j < temp.size(); j++)
        {
            temp[0] = bitsetConcat(temp[0], temp[j]);
        }

        vv[i] = temp[0];
    }

    return vv;
}

template <std::size_t N>
vector<vector<bitset<N>>> bitsetBoundedEventually(vector<vector<bitset<N>>> &v1, vector<long long> segmentation, const long long &a, const long long &b, const bool &leftClosed, const bool &rightClosed)
{
    vector<vector<bitset<N>>> vv(v1.size());

    for (auto &v : vv)
    {
        v.resize(2);
    }

    for (int i = 0; i < v1.size(); i++)
    {
        vector<vector<bitset<N>>> temp = getProfiles(v1, segmentation, segmentation[i], segmentation[i + 1], a, b, leftClosed, rightClosed);

        temp = bitsetEventually(temp);
        for (int j = 1; j < temp.size(); j++)
        {
            temp[0] = bitsetConcat(temp[0], temp[j]);
        }

        vv[i] = temp[0];
    }

    return vv;
}

template <std::size_t N>
vector<vector<bitset<N>>> bitsetUntil(vector<vector<bitset<N>>> v1, vector<vector<bitset<N>>> v2)
{
    vector<vector<bitset<N>>> vv(v1.size());

    for (auto &v : vv)
    {
        v.resize(2);
    }

    bool firstBit0 = false;
    bool firstBit1 = true;

    for (int i = v1.size() - 1; i >= 0; i--)
    {
        // TODO: improve this
        vector<bool> save(4);
        save[0] = v1[i][0][0];
        save[1] = v1[i][1][0];
        save[2] = v2[i][0][0];
        save[3] = v2[i][1][0];

        if (firstBit0 == true)
        {
            // handling the corner cases of 0 U x, 1 U x, x U 0, x U 1
            if (v1[i][0][0] == true)
            {
                vv[i][0] = vv[i][0] | v2[i][0];
                vv[i][1] = vv[i][1] | v2[i][1];
                v1[i][0][0] = false;
            }

            if (v1[i][1][0] == true)
            {
                vector<int> temp(4);
                temp[0] = msb(v2[i][0] & evenMask) + 1; // 0...0
                temp[1] = msb(v2[i][0] & oddMask) + 1;  // 0...1
                temp[2] = msb(v2[i][1] & oddMask) + 1;  // 1...0
                temp[3] = msb(v2[i][1] & evenMask) + 1; // 1...1

                vv[i][0][0] = vv[i][0][0] || v2[i][0][0];
                vv[i][1][0] = vv[i][1][0] || (max(temp[1], temp[3]) > 0);
                vv[i][1][1] = vv[i][1][1] || (max(temp[0] - 2, temp[2]) > 0);

                v1[i][1][0] = false;
            }

            if (v2[i][0][0] == true)
            {
                vv[i][0][0] = true;
                v2[i][0][0] = false;
            }

            if (v2[i][1][0] == true)
            {
                vv[i][1][0] = true;
                v2[i][1][0] = false;
            }

            if ((v1[i][0].none() && v1[i][1].none()) || (v2[i][0].none() && v2[i][1].none()))
            {
                continue;
            }

            vector<int> len1(4);
            len1[0] = msb(v1[i][0] & evenMask) + 1; // 0...0
            len1[1] = msb(v1[i][0] & oddMask) + 1;  // 0...1
            len1[2] = msb(v1[i][1] & oddMask) + 1;  // 1...0
            len1[3] = msb(v1[i][1] & evenMask) + 1; // 1...1

            vector<int> len2(4);
            len2[0] = msb(v2[i][0] & evenMask) + 1; // 0...0
            len2[1] = msb(v2[i][0] & oddMask) + 1;  // 0...1
            len2[2] = msb(v2[i][1] & oddMask) + 1;  // 1...0
            len2[3] = msb(v2[i][1] & evenMask) + 1; // 1...1

            vector<int> len_strong(4);
            len_strong[0] = len2[0];
            len_strong[1] = len2[1];
            len_strong[2] = (max(len1[2], len1[3]) > 0) ? max(len2[2], len2[0] - 1) : len2[2];
            len_strong[3] = (max(len1[2], len1[3]) > 0) ? max(len2[3], len2[1] - 1) : len2[3];

            int ctr;

            // 0...0
            ctr = len_strong[0] - 1;
            if (ctr > -1)
            {
                if (len1[2] == 2 && len1[0] == 0 && len1[1] == 0 && len1[3] == 0)
                {
                    while (ctr > 0)
                    {
                        vv[i][0][ctr] = v2[i][0][ctr];
                        ctr -= 2;
                    }
                }
                else
                {
                    while (ctr > 0)
                    {
                        vv[i][0][ctr] = true;
                        ctr -= 2;
                    }
                }
            }

            // 0...1
            ctr = len_strong[1] - 1;
            if (ctr > -1)
            {
                if (len1[2] == 2 && len1[0] == 0 && len1[1] == 0 && len1[3] == 0)
                {
                    while (ctr > 0)
                    {
                        vv[i][0][ctr] = v2[i][0][ctr];
                        ctr -= 2;
                    }
                }
                else
                {
                    while (ctr > 0)
                    {
                        vv[i][0][ctr] = true;
                        ctr -= 2;
                    }
                }
            }

            // 1...0
            ctr = len_strong[2] - 1;
            while (ctr > 0)
            {
                vv[i][1][ctr] = true;
                ctr -= 2;
            }

            // 1...1
            ctr = len_strong[3] - 1;
            while (ctr >= 0)
            {
                vv[i][1][ctr] = true;
                ctr -= 2;
            }
        }

        if (firstBit1 == true)
        {
            v1[i][0][0] = save[0];
            v1[i][1][0] = save[1];
            v2[i][0][0] = save[2];
            v2[i][1][0] = save[3];

            // handling the corner cases of 0Ux=x, 1Ux=1, xU0=Ax, xU1=1
            if (v2[i][0][0] == true)
            {
                vector<int> temp(4);
                temp[0] = msb(v1[i][0] & evenMask) + 1; // 0...0
                temp[1] = msb(v1[i][0] & oddMask) + 1;  // 0...1
                temp[2] = msb(v1[i][1] & oddMask) + 1;  // 1...0
                temp[3] = msb(v1[i][1] & evenMask) + 1; // 1...1

                vv[i][0][0] = vv[i][0][0] || (max(temp[0], temp[2]) > 0);
                vv[i][0][1] = vv[i][0][1] || (max(temp[1], temp[3] - 2) > 0);
                vv[i][1][0] = vv[i][1][0] || v1[i][1][0];

                v2[i][0][0] = false;
            }

            if (v1[i][0][0] == true)
            {
                v2[i][0][0] = save[2];

                vv[i][0] = vv[i][0] | v2[i][0];
                vv[i][1] = vv[i][1] | v2[i][1];
                v1[i][0][0] = false;

                v2[i][0][0] = false;
            }

            if (v1[i][1][0] == true)
            {
                vv[i][1][0] = true;
                v1[i][1][0] = false;
            }

            if (v2[i][1][0] == true)
            {
                vv[i][1][0] = true;
                v2[i][1][0] = false;
            }

            if ((v1[i][0].none() && v1[i][1].none()) || (v2[i][0].none() && v2[i][1].none()))
            {
                continue;
            }

            vector<int> len1(4);
            len1[0] = msb(v1[i][0] & evenMask) + 1; // 0...0
            len1[1] = msb(v1[i][0] & oddMask) + 1;  // 0...1
            len1[2] = msb(v1[i][1] & oddMask) + 1;  // 1...0
            len1[3] = msb(v1[i][1] & evenMask) + 1; // 1...1

            vector<int> len2(4);
            len2[0] = msb(v2[i][0] & evenMask) + 1; // 0...0
            len2[1] = msb(v2[i][0] & oddMask) + 1;  // 0...1
            len2[2] = msb(v2[i][1] & oddMask) + 1;  // 1...0
            len2[3] = msb(v2[i][1] & evenMask) + 1; // 1...1

            vector<int> len_weak(4);
            len_weak[0] = (max(len1[0], len1[2]) > 0) ? (len2[0]) : 0;
            len_weak[1] = ((max(len1[1], len1[3]) > 0) && len2[0] > 0) ? (max(len2[0] + 1, len2[1])) : len2[1];

            if (len1[0] > 0 || len1[2] > 0)
            {
                if (len1[2] > 0 && len2[0] > 0)
                {
                    len_weak[2] = max(len2[2], len2[0] - 1);
                }
                else
                {
                    len_weak[2] = len2[2];
                }
            }
            else
            {
                len_weak[2] = 0;
            }

            if (len1[3] > 0)
            {
                if (len2[2] > 0)
                {
                    len_weak[3] = max({len2[0], len2[1] - 1, len2[2] + 1, len2[3]});
                }
                else
                {
                    len_weak[3] = max({len2[0], len2[1] - 1, len2[3]});
                }
            }
            else if (len1[2] > 0 && len1[1] > 0)
            {
                if (len2[2] > 0)
                {
                    len_weak[3] = max({len2[1] - 1, len2[2] + 1, len2[3]});
                }
                else
                {
                    len_weak[3] = max(len2[1] - 1, len2[3]);
                }
            }
            else if (len1[2] > 0)
            {
                len_weak[3] = max(len2[1] - 1, len2[3]);
            }
            else if (len1[1] > 0)
            {
                if (len2[2] > 0)
                {
                    len_weak[3] = max(len2[2] + 1, len2[3]);
                }
                else
                {
                    len_weak[3] = len2[3];
                }
            }
            else
            {
                len_weak[3] = len2[3];
            }

            int ctr;

            // 0...0
            ctr = len_weak[0] - 1;
            if (ctr > -1)
            {
                if (len1[2] == 2 && len1[0] == 0)
                {
                    while (ctr > 0)
                    {
                        vv[i][0][ctr] = v2[i][0][ctr];
                        ctr -= 2;
                    }
                }
                else
                {
                    while (ctr > 0)
                    {
                        vv[i][0][ctr] = true;
                        ctr -= 2;
                    }
                }
            }

            // 0...1
            ctr = len_weak[1] - 1;
            if (ctr > -1)
            {
                if (len1[2] == 2 && len1[0] == 0 && len1[1] == 0 && len1[3] == 0)
                {
                    while (ctr > 0)
                    {
                        vv[i][0][ctr] = v2[i][0][ctr];
                        ctr -= 2;
                    }
                }
                else
                {
                    while (ctr > 0)
                    {
                        vv[i][0][ctr] = true;
                        ctr -= 2;
                    }
                }
            }

            // 1...0
            ctr = len_weak[2] - 1;
            if (ctr > -1)
            {
                if (len1[2] == 2 && len1[0] == 0 && len2[2] == 0)
                {
                    while (ctr > 0)
                    {
                        vv[i][1][ctr] = v2[i][1][ctr];
                        ctr -= 2;
                    }
                    int cc = len2[0] - 2;
                    while (cc > 0)
                    {
                        vv[i][1][cc] = true;
                        cc -= 2;
                    }
                }
                else
                {
                    while (ctr > 0)
                    {
                        vv[i][1][ctr] = true;
                        ctr -= 2;
                    }
                }
            }

            // 1...1
            ctr = len_weak[3] - 1;
            while (ctr >= 0)
            {
                vv[i][1][ctr] = true;
                ctr -= 2;
            }
        }
    }

    return vv;
}

template <std::size_t N>
vector<vector<bitset<N>>> bitsetConjunction(vector<vector<bitset<N>>> v1, vector<vector<bitset<N>>> v2)
{
    vector<vector<bitset<N>>> vv(v1.size());

    for (auto &v : vv)
    {
        v.resize(2);
    }

    for (int i = 0; i < v1.size(); i++)
    {
        // handling the corner cases of conjunction with 0 or 1
        if (v1[i][0][0] == true)
        {
            vv[i][0][0] = true;
            v1[i][0][0] = false;
        }
        if (v2[i][0][0] == true)
        {
            vv[i][0][0] = true;
            v2[i][0][0] = false;
        }
        if (v1[i][1][0] == true)
        {
            vv[i][0] = vv[i][0] | v2[i][0];
            vv[i][1] = vv[i][1] | v2[i][1];
            v1[i][1][0] = false;
        }
        if (v2[i][1][0] == true)
        {
            vv[i][0] = vv[i][0] | v1[i][0];
            vv[i][1] = vv[i][1] | v1[i][1];
            v2[i][1][0] = false;
        }

        vector<int> len1(4);
        len1[0] = msb(v1[i][0] & evenMask) + 1; // 0...0
        len1[1] = msb(v1[i][0] & oddMask) + 1;  // 0...1
        len1[2] = msb(v1[i][1] & oddMask) + 1;  // 1...0
        len1[3] = msb(v1[i][1] & evenMask) + 1; // 1...1

        vector<int> len2(4);
        len2[0] = msb(v2[i][0] & evenMask) + 1; // 0...0
        len2[1] = msb(v2[i][0] & oddMask) + 1;  // 0...1
        len2[2] = msb(v2[i][1] & oddMask) + 1;  // 1...0
        len2[3] = msb(v2[i][1] & evenMask) + 1; // 1...1

        for (int i = 0; i < 4; i++)
        {
            if (len1[i] == 0)
            {
                len1[i] = INT32_MIN / 2 + 2;
            }

            if (len2[i] == 0)
            {
                len2[i] = INT32_MIN / 2 + 2;
            }
        }

        vector<int> len(4);
        len[0] = max({len1[0] + len2[0] - 2, len1[0] + len2[1] - 1, len1[0] + len2[2] - 1, len1[0] + len2[3], len1[1] + len2[0] - 1, len1[1] + len2[2], len1[2] + len2[0] - 1, len1[2] + len2[1], len1[3] + len2[0]}) - 1;
        len[1] = max({len1[1] + len2[1] - 1, len1[1] + len2[3], len1[3] + len2[1]}) - 1;
        len[2] = max({len1[2] + len2[2] - 1, len1[2] + len2[3], len1[3] + len2[2]}) - 1;
        len[3] = len1[3] + len2[3] - 1;

        int ctr;
        for (int j = 0; j < 3; j++)
        {
            ctr = len[j] - 1;

            while (ctr >= 0)
            {
                vv[i][j / 2][ctr] = true;
                ctr = ctr - 2;
            }
        }
        ctr = len[3] - 1;
        while (ctr > 0)
        {
            vv[i][1][ctr] = true;
            ctr = ctr - 2;
        }
    }

    return vv;
}

template <std::size_t N>
vector<vector<bitset<N>>> bitsetNegation(vector<vector<bitset<N>>> v)
{
    for (auto &vv : v)
    {
        swap(vv[0], vv[1]);
    }

    return v;
}

// this only works for boolean signals
template <std::size_t N>
vector<vector<vector<bitset<N>>>> convertIntoBitset(vector<vector<set<string>>> &valExprs)
{
    vector<vector<vector<bitset<N>>>> aps(valExprs.size());

    for (int i = 0; i < valExprs.size(); i++)
    {
        aps[i].resize(valExprs[i].size());

        for (int j = 0; j < valExprs[i].size(); j++)
        {
            aps[i][j].resize(2);

            for (auto s : valExprs[i][j])
            {
                if (!s.empty())
                {
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

set<string> concatWithDestutter(const set<string> &set1, const set<string> &set2)
{
    set<string> result;

    for (const auto &s1 : set1)
    {
        for (const auto &s2 : set2)
        {
            if (!s1.empty() && !s2.empty())
            {
                if (s1.rfind(';') == string::npos && s2.find(';') == string::npos)
                {
                    if (s1 != s2)
                    {
                        result.insert(s1 + ";" + s2);
                    }
                    else
                    {
                        result.insert(s1);
                    }
                }
                else if (s1.rfind(';') == string::npos && s2.find(';') != string::npos)
                {
                    if (s1 != s2.substr(0, s2.find(';')))
                    {
                        result.insert(s1 + ";" + s2);
                    }
                    else
                    {
                        result.insert(s2);
                    }
                }
                else if (s1.rfind(';') != string::npos && s2.find(';') == string::npos)
                {
                    if (s1.substr(s1.rfind(';') + 1, s1.length()) != s2)
                    {
                        result.insert(s1 + ";" + s2);
                    }
                    else
                    {
                        result.insert(s1);
                    }
                }
                else
                {
                    if (s1.substr(s1.rfind(';') + 1, s1.length()) != s2.substr(0, s2.find(';')))
                    {
                        result.insert(s1 + ";" + s2);
                    }
                    else
                    {
                        result.insert(s1 + ";" + s2.substr(s2.find(';') + 1, s2.length()));
                    }
                }
            }
            else
            {
                if (!s1.empty())
                {
                    result.insert(s1);
                }
                else
                {
                    result.insert(s2);
                }
            }
        }
    }

    return result;
}

pair<int, int> convertIntoBoolWithDestutter(string str, string delimiter)
{
    vector<bool> v;

    if (!str.empty())
    {
        int start = 0;
        do
        {
            // Find the index of occurrence
            int idx = str.find(delimiter, start);
            if (idx == string::npos)
            {
                break;
            }

            int length = idx - start;
            if (stof(str.substr(start, length)) > 0)
            {
                if (v.empty() || v[v.size() - 1] != true)
                {
                    v.push_back(true);
                }
            }
            else
            {
                if (v.empty() || v[v.size() - 1] != false)
                {
                    v.push_back(false);
                }
            }
            start += (length + delimiter.size());
        } while (true);

        if (stof(str.substr(start, str.length())) > 0)
        {
            if (v.empty() || v[v.size() - 1] != true)
            {
                v.push_back(true);
            }
        }
        else
        {
            if (v.empty() || v[v.size() - 1] != false)
            {
                v.push_back(false);
            }
        }
    }

    int x = -1;
    int y = -1;

    if (!v.empty())
    {
        if (v[0] == true)
        {
            x = 1;
        }
        else
        {
            x = 0;
        }

        y = v.size() - 1;
    }

    return make_pair(x, y);
}

int main()
{
    vector<long long> D{4000, 8000, 16000};
    vector<long long> EPS{1000, 2000, 4000, 8000, 16000};
    vector<long long> DEL{1, 1000, 2000, 4000, 8000, 16000};

    /*
    long long a = 0;
    long long b = 16000;
    bool rightClosed = true;
    bool leftClosed = false;
    */

    // CHANGE THIS
    // int n = 1;
    int n = 2;

    ofstream results;
    string filename = "results_ad.txt";
    results.open(filename);

    for (const auto &d : D)
    {
        for (const auto &eps : EPS)
        {
            for (const auto &del : DEL)
            {
                if (eps < del)
                {
                    continue;
                }

                for (int c = 0; c < 100; c++)
                {
                    vector<vector<pair<long long, double>>> signalsReal(n);
                    string filename1 = "dataNew/" + to_string(d / 1000) + "_" + to_string(c) + ".txt";
                    ifstream sigdata1(filename1);

                    string line1;
                    while (getline(sigdata1, line1))
                    {
                        stringstream linestream(line1);
                        double t, v;
                        linestream >> t >> v;
                        signalsReal[0].push_back(make_pair((long long)(t * 1000), v));
                    }
                    sigdata1.close();

                    signalsReal[1].clear();
                    string filename2 = "dataNew/" + to_string(d / 1000) + "_" + to_string(c + 100) + ".txt";
                    ifstream sigdata2(filename2);

                    string line2;
                    while (getline(sigdata2, line2))
                    {
                        stringstream linestream(line2);
                        double t, v;
                        linestream >> t >> v;
                        signalsReal[1].push_back(make_pair((long long)(t * 1000), v));
                    }
                    sigdata2.close();

                    int numSegments;
                    bool error = false;
                    starttime = chrono::system_clock::now();
                    for (int rep = 0; rep < REP; rep++)
                    {
                        /* read the signals as atomic propositions to improve performance */
                        vector<vector<pair<long long, double>>> signals;
                        // signals = signalsReal;
                        signals.resize(n);
                        for (int i = 0; i < n; i++)
                        {
                            signals[i].push_back(make_pair(0, signalsReal[i][0].second));
                            for (int j = 1; j < signalsReal[i].size(); j++)
                            {
                                if (signalsReal[i][j].second > 0 && signalsReal[i][j - 1].second <= 0)
                                {
                                    signals[i].push_back(make_pair(signalsReal[i][j].first, 1));
                                }
                                else if (signalsReal[i][j].second <= 0 && signalsReal[i][j - 1].second > 0)
                                {
                                    signals[i].push_back(make_pair(signalsReal[i][j].first, 0));
                                }
                            }
                        }

                        /* compute the uncertainty intervals */
                        vector<vector<vector<long long>>> uncertainties(n);

                        for (int i = 0; i < n; i++)
                        {
                            int gi = signals[i].size();
                            uncertainties[i].resize(gi);

                            uncertainties[i][0].push_back(-del);
                            for (int j = 1; j < gi; j++)
                            {
                                long long tj = signals[i][j].first;
                                long long low = max(uncertainties[i][j - 1][0] + del, tj - eps);
                                uncertainties[i][j].push_back(low);
                            }

                            uncertainties[i][gi - 1].push_back(signals[i][gi - 1].first + eps);
                            for (int j = gi - 2; j > 0; j--)
                            {
                                long long tj = signals[i][j].first;
                                long long high = min(uncertainties[i][j + 1][1] - del, tj + eps);
                                uncertainties[i][j].push_back(high);
                            }

                            for (int j = 1; j < gi; j++)
                            {
                                if (uncertainties[i][j][0] >= uncertainties[i][j][1])
                                {
                                    error = true;
                                    break;
                                }
                            }

                            if (error)
                            {
                                break;
                            }
                        }

                        if (error)
                        {
                            break;
                        }

                        /* compute the canonical segmentation */
                        set<long long> segmentation_temp;

                        segmentation_temp.insert(0);
                        segmentation_temp.insert(max(signals[0][signals[0].size() - 1].first, (long long)(d)));

                        for (const auto &s : uncertainties)
                        {
                            for (int j = 1; j < s.size(); j++)
                            {
                                for (auto b : s[j])
                                {
                                    segmentation_temp.insert(b);
                                }
                            }
                        }

                        vector<long long> segmentation;
                        for (const auto &b : segmentation_temp)
                        {
                            segmentation.push_back(b);
                        }

                        numSegments = segmentation.size() - 1;

                        /* compute the value expressions */
                        // TODO: check and improve
                        vector<vector<set<string>>> valExprs(n);

                        for (int i = 0; i < n; i++)
                        {
                            valExprs[i].resize(numSegments);

                            for (int j = 0; j < numSegments; j++)
                            {
                                valExprs[i][j].insert("");

                                for (int k = 1; k < uncertainties[i].size(); k++)
                                {
                                    if (segmentation[j] == uncertainties[i][k][0] && segmentation[j + 1] == uncertainties[i][k][1])
                                    {
                                        // entire expression
                                        set<string> temp;
                                        string expr = to_string(signals[i][k - 1].second) + ";" + to_string(signals[i][k].second);
                                        temp.insert("");
                                        temp.insert(expr);
                                        valExprs[i][j] = concatWithDestutter(valExprs[i][j], temp);
                                    }
                                    else if (segmentation[j] == uncertainties[i][k][0] && segmentation[j + 1] < uncertainties[i][k][1])
                                    {
                                        // prefix
                                        set<string> temp;
                                        string expr = to_string(signals[i][k - 1].second) + ";" + to_string(signals[i][k].second);
                                        temp.insert("");
                                        temp.insert(expr);
                                        temp.insert(expr.substr(0, expr.find(';')));
                                        valExprs[i][j] = concatWithDestutter(valExprs[i][j], temp);
                                    }
                                    else if (segmentation[j] > uncertainties[i][k][0] && segmentation[j + 1] == uncertainties[i][k][1])
                                    {
                                        // suffix
                                        set<string> temp;
                                        string expr = to_string(signals[i][k - 1].second) + ";" + to_string(signals[i][k].second);
                                        temp.insert("");
                                        temp.insert(expr);
                                        temp.insert(expr.substr(expr.find(';') + 1, expr.length()));
                                        valExprs[i][j] = concatWithDestutter(valExprs[i][j], temp);
                                    }
                                    else if (segmentation[j] > uncertainties[i][k][0] && segmentation[j + 1] < uncertainties[i][k][1])
                                    {
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

                                if (valExprs[i][j].size() == 1)
                                {
                                    int kk = 1;
                                    while (kk < uncertainties[i].size() && uncertainties[i][kk][1] <= segmentation[j])
                                    {
                                        kk++;
                                    }
                                    valExprs[i][j].insert(to_string(signals[i][kk - 1].second));
                                }
                            }
                        }

                        /* translate real-valued signals to atomic propositions */
                        vector<vector<vector<bitset<SIZE>>>> aps(n);
                        for (auto &v : aps)
                        {
                            v.resize(numSegments);
                            for (auto &vv : v)
                            {
                                vv.resize(2);
                            }
                        }

                        for (int i = 0; i < n; i++)
                        {
                            for (int j = 0; j < numSegments; j++)
                            {
                                for (auto &expr : valExprs[i][j])
                                {
                                    if (expr != "")
                                    {
                                        pair<int, int> xy = convertIntoBoolWithDestutter(expr, ";");
                                        if (xy.first >= 0 && xy.second >= 0)
                                        {
                                            aps[i][j][xy.first][xy.second] = true;
                                        }
                                    }
                                }
                            }
                        }

                        for (int i = 0; i < SIZE; i++)
                        {
                            if (i % 2 == 0)
                            {
                                evenMask[i] = true;
                            }
                            else
                            {
                                oddMask[i] = true;
                            }
                        }

                        // always(x1 and x2)
                        // test = bitsetAlways(bitsetConjunction(aps[0], aps[1]));

                        // always(x1 or x2)
                        test = bitsetAlways(bitsetNegation(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1]))));

                        // eventually(x1 and x2)
                        // test = bitsetEventually(bitsetConjunction(aps[0], aps[1]));

                        // eventually(x1 or x2)
                        // test = bitsetEventually(bitsetNegation(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1]))));

                        // x1 until x2
                        // test = bitsetUntil(aps[0], aps[1]);
                    }

                    endtime = chrono::system_clock::now();
                    chrono::duration<double, milli> totalTime = endtime - starttime;

                    bool falseAtZero = test[0][0].any();
                    bool trueAtZero = test[0][1].any();

                    string out = "";
                    if (falseAtZero && trueAtZero)
                    {
                        out = "2";
                    }
                    else if (trueAtZero)
                    {
                        out = "1";
                    }
                    else
                    {
                        out = "0";
                    }

                    string w;
                    if (error)
                    {
                        w = to_string(d / 1000) + " " + to_string(eps / 1000) + " " + to_string(del / 1000) + " " + to_string(c) + " " + "-" + " " + "-" + " " + "-";
                    }
                    else
                    {
                        w = to_string(d / 1000) + " " + to_string(eps / 1000) + " " + to_string(del / 1000) + " " + to_string(c) + " " + to_string(numSegments) + " " + to_string((totalTime.count() / REP) / 1000) + " " + out;
                    }
                    results << w << endl;
                    cout << w << endl;
                }
            }
        }
    }

    results.close();

    return 0;
}
