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
#include "functions.cpp"
using namespace std;

#define REP 1

int main()
{
    /* set variables */
    int n = 2;
    int d = 4000;
    int eps = 1000;
    int del = 1;
    /*
    long long a = 0;
    long long b = 8000;
    bool leftClosed = true;
    bool rightClosed = false;
    */

    /* read signal data */
    // vector<vector<pair<long long, double>>> signalsReal(n);
    // signalsReal[0] = getData("dataNew/4_23.txt");
    // signalsReal[1] = getData("dataNew/4_123.txt");
    
    /* prepare the bit masks */
    evenMask = generateBitmask(0);
    oddMask = generateBitmask(1);

    // int numSegments;
    // vector<vector<bitset<SIZE>>> test;
    chrono::time_point<chrono::system_clock> starttime;
    chrono::time_point<chrono::system_clock> endtime; 
    
    starttime = chrono::system_clock::now();
    for (int rep = 0; rep < REP; rep++)
    {
        /* read the signals as atomic propositions to improve performance */
        // vector<vector<pair<long long, double>>> signals = convertSignalsToBool(signalsReal);

        /* compute the uncertainty intervals */
        // vector<vector<vector<long long>>> uncertainties = computeUncertaintyIntervals(signals, eps, del);

        /* compute the canonical segmentation */
        // vector<long long> segmentation = computeCanonicalSegmentation(signals, uncertainties, d);
        // numSegments = segmentation.size() - 1;

        /* compute the value expressions */
        // vector<vector<set<string>>> valExprs = computeValueExpressions(signals, uncertainties, segmentation);

        /* translate signals to atomic propositions */
        // vector<vector<vector<bitset<SIZE>>>> aps = convertSignalsToAtomicPropositions(valExprs);

        /* evaluate the formula */
        // test = bitsetConjunction(aps[0], aps[1]);
        // test = bitsetEventually(test);
        // test = bitsetEventually(bitsetNegation(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1]))));
        // test = bitsetAlways(bitsetNegation(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1]))));
        ////test = bitsetNegation(bitsetAlways(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1]))));
        // test = bitsetUntilNonStrict(aps[0], aps[1]);
        // test = bitsetBoundedEventually(aps[0], segmentation, a, b, leftClosed, rightClosed);

        vector<vector<vector<bitset<SIZE>>>> aps(n);
        for (auto &v : aps) {
            v.resize(2);
            v[0].resize(2);
            v[1].resize(2);
        }

        int up = 8;
        for (int i = 0; i < up; i++) 
        {
            aps[0][0][0] = bitset<SIZE>(i);
            for (int j = 0; j < up; j++) 
            {
                aps[0][0][1] = bitset<SIZE>(j);
                for (int k = 0; k < up; k++) 
                {
                    aps[0][1][0] = bitset<SIZE>(k);
                    for (int l = 0; l < up; l++) 
                    {
                        aps[0][1][1] = bitset<SIZE>(l);
                        for (int ii = 0; ii < up; ii++) 
                        {
                            aps[1][0][0] = bitset<SIZE>(ii);
                            for (int jj = 0; jj < up; jj++) 
                            {
                                aps[1][0][1] = bitset<SIZE>(jj);
                                for (int kk = 0; kk < up; kk++) 
                                {
                                    aps[1][1][0] = bitset<SIZE>(kk);
                                    for (int ll = 0; ll < up; ll++) 
                                    {
                                        aps[1][1][1] = bitset<SIZE>(ll);

                                        if ((i > 0 || j > 0) && (k > 0 || l > 0) && (ii > 0 || jj > 0) && (kk > 0 || ll > 0)) {
                                            cout << i << " " << j << " " << k << " " << l << " :: " << ii << " " << jj << " " << kk << " " << ll << endl;
                                            vector<set<pair<string, string>>> prod = asyncProd(aps[0], aps[1]);
                                            vector<set<string>> prodTest = prodUntilStrict(prod, false); // pass 'false' for strong until, 'true' for weak until
                                            vector<vector<bitset<SIZE>>> test = bitsetUntilStrict(aps[0], aps[1]);
                                            
                                            if (!isEqual(prodTest, test)) {
                                                cout << "error" << endl;
                                            }
                                        }
                                        
                                    } 
                                } 
                            }
                        }
                    } 
                } 
            }
        }

        /*
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                cout << i << " " << j << endl;
                
                int x = (i < 2) ? 0 : 1;
                int y = (j < 2) ? 0 : 1;
                int px = (x + i) % 2;
                int py = (y + j) % 2;
                if (px == 0) px = 2;
                if (py == 0) py = 2;

                for (int k = px; k <= 6; k = k + 2) {
                    for (int l = py; l <= 6; l = l + 2) {
                        cout << ":: " << k << " " << l << endl; 
                            vector<vector<vector<bitset<SIZE>>>> aps(n);
                            for (auto &v : aps) {
                                v.resize(1);
                                v[0].resize(2);
                            }
                            aps[0][0][x] = bitset<SIZE>("1" + string(k, '0'));
                            aps[0][0][1 - x] = bitset<SIZE>("0");
                            aps[1][0][y] = bitset<SIZE>("1" + string(l, '0'));
                            aps[1][0][1 - y] = bitset<SIZE>("0");

                            vector<set<pair<string, string>>> prod = asyncProd(aps[0], aps[1]);
                            vector<set<string>> prodTest = prodUntilStrict(prod, true); // pass 'false' for strong until, 'true' for weak until
                            vector<vector<bitset<SIZE>>> test = bitsetUntilStrict(aps[0], aps[1]);
                            
                            if (!isEqual(prodTest, test)) {
                                cout << "error" << endl;
                            }
                            
                            // cout << k << " " << l << ": "; 
                            // for (auto s : prodTest[0]) {
                            //     cout << s << " ";
                            // }
                            // cout << endl;
                    }
                }

                // cout << endl;
            }
        }
        */

        
        /*
        aps[0][0][0] = bitset<SIZE>("1000000");
        aps[0][0][1] = bitset<SIZE>("0");
        aps[1][0][0] = bitset<SIZE>("0");
        aps[1][0][1] = bitset<SIZE>("100");

        vector<set<pair<string, string>>> prod = asyncProd(aps[0], aps[1]);
        vector<set<string>> prodTest = prodUntilStrict(prod, true); // pass 'false' for strong until, 'true' for weak until

        // cout << isEqual(prodTest, test); 
        endtime = chrono::system_clock::now();
        chrono::duration<double, milli> totalTime = endtime - starttime;

       // cout << d << " " << eps << " " << del << " " << numSegments << " " << (totalTime.count() / REP) / 1000 << " " << test[0][0].any() << " " << test[0][1].any() << endl;
       */
    }

    return 0;
}