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
    int eps = 4000;
    int del = 1;
    /*
    long long a = 0;
    long long b = 8000;
    bool leftClosed = true;
    bool rightClosed = false;
    */

    /* read signal data */
    vector<vector<pair<long long, double>>> signalsReal(n);
    signalsReal[0] = getData("dataNew/4_73.txt");
    signalsReal[1] = getData("dataNew/4_173.txt");
    
    /* prepare the bit masks */
    bitset<SIZE> evenMask = generateBitmask(0);
    bitset<SIZE> oddMask = generateBitmask(1);

    int numSegments;
    vector<vector<bitset<SIZE>>> test;
    chrono::time_point<chrono::system_clock> starttime;
    chrono::time_point<chrono::system_clock> endtime; 
    
    starttime = chrono::system_clock::now();
    for (int rep = 0; rep < REP; rep++)
    {
        /* read the signals as atomic propositions to improve performance */
        vector<vector<pair<long long, double>>> signals = convertSignalsToBool(signalsReal);

        /* compute the uncertainty intervals */
        vector<vector<vector<long long>>> uncertainties = computeUncertaintyIntervals(signals, eps, del);

        /* compute the canonical segmentation */
        vector<long long> segmentation = computeCanonicalSegmentation(signals, uncertainties, d);
        numSegments = segmentation.size() - 1;

        /* compute the value expressions */
        vector<vector<set<string>>> valExprs = computeValueExpressions(signals, uncertainties, segmentation);

        /* translate signals to atomic propositions */
        vector<vector<vector<bitset<SIZE>>>> aps = convertSignalsToAtomicPropositions(valExprs);

        /* evaluate the formula */
        // test = bitsetConjunction(aps[0], aps[1]);
        // test = bitsetEventually(test);
        // test = bitsetEventually(bitsetNegation(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1])))); // this is faster
        // test = bitsetAlways(bitsetNegation(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1])))); // this is faster
        ////test = bitsetNegation(bitsetAlways(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1]))));
        test = bitsetUntil(aps[0], aps[1]);
        // test = bitsetBoundedEventually(aps[0], segmentation, a, b, leftClosed, rightClosed);

        endtime = chrono::system_clock::now();
        chrono::duration<double, milli> totalTime = endtime - starttime;

        cout << d << " " << eps << " " << del << " " << numSegments << " " << (totalTime.count() / REP) / 1000 << " " << test[0][0].any() << " " << test[0][1].any() << endl;
    }

    return 0;
}