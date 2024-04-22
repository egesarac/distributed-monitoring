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
    int n = 3;
    int d = 5000;
    int eps = 50 * 8;
    int del = 1;
    /*
    long long a = 0;
    long long b = 8000;
    bool leftClosed = true;
    bool rightClosed = false;
    */

    /* read signal data */
    vector<vector<pair<long long, double>>> signalsReal(n);
    for (int i = 0; i < n; i++)
    {
        signalsReal[i] = getData("data_re/wt/s5_tank_" + to_string(i));
    }
    
    /* prepare the bit masks */
    evenMask = generateBitmask(0);
    oddMask = generateBitmask(1);

    int numSegments;
    vector<vector<bitset<SIZE>>> test;
    chrono::time_point<chrono::system_clock> starttime;
    chrono::time_point<chrono::system_clock> endtime; 
    chrono::duration<double, milli> totalTime;
    
    starttime = chrono::system_clock::now();
    for (int rep = 0; rep < REP; rep++)
    {
        /* read the signals as atomic propositions to improve performance */
        //vector<vector<pair<long long, double>>> signals = convertSignalsToBool(signalsReal);

        /* compute the uncertainty intervals */
        vector<vector<vector<long long>>> uncertainties = computeUncertaintyIntervals(signalsReal, eps, del);

        /* compute the canonical segmentation */
        vector<long long> segmentation = computeCanonicalSegmentation(signalsReal, uncertainties, d);
        numSegments = segmentation.size() - 1;

        /* compute the value expressions */
        vector<vector<set<string>>> valExprs = computeValueExpressions(signalsReal, uncertainties, segmentation);
        vector<set<string>> ve = valExprs[0];
        for (int i = 1; i < n; i++)
        {
            // ve = asyncProdStrSum(ve, valExprs[i]);
            ve = abstProdCoarseStrSum(ve, valExprs[i]);
            // ve = abstProdStrSum(ve, valExprs[i]);

        }
        valExprs = {ve};

        /* translate signals to atomic propositions */
        vector<vector<vector<bitset<SIZE>>>> aps = convertSignalsToAtomicPropositions(valExprs, 10.0);

        /* evaluate the formula */
        test = bitsetAlways(aps[0]);
        // test = bitsetConjunction(aps[0], aps[1]);
        // test = bitsetEventually(test);
        // test = bitsetEventually(bitsetNegation(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1])))); // this is faster
        // test = bitsetAlways(bitsetNegation(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1])))); // this is faster
        ////test = bitsetNegation(bitsetAlways(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1]))));
        // test = bitsetUntilStrict(aps[0], aps[1]);
        // test = bitsetBoundedEventually(aps[0], segmentation, a, b, leftClosed, rightClosed);

    }
    
    endtime = chrono::system_clock::now();
    totalTime += endtime - starttime;

    cout << d << " " << eps << " " << del << " " << numSegments << " " << (totalTime.count() / REP) / 1000 << " " << test[0][0].any() << " " << test[0][1].any() << endl;

    return 0;
}