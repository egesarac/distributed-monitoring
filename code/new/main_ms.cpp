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
    int d = 1000;
    int eps = 50;
    int del = 1;
    /*
    long long a = 0;
    long long b = 8000;
    bool leftClosed = true;
    bool rightClosed = false;
    */

    /* read signal data */
    vector<vector<pair<long long, double>>> signalsReal(3*n);
    signalsReal[0] = getData3d("data_re/uav/s1_uav_0", 0);
    signalsReal[1] = getData3d("data_re/uav/s1_uav_0", 1);
    signalsReal[2] = getData3d("data_re/uav/s1_uav_0", 2);
    signalsReal[3] = getData3d("data_re/uav/s1_uav_1", 0);
    signalsReal[4] = getData3d("data_re/uav/s1_uav_1", 1);
    signalsReal[5] = getData3d("data_re/uav/s1_uav_1", 2);

    
    
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
        //vector<vector<pair<long long, double>>> signals = convertSignalsToBool(signalsReal);

        /* compute the uncertainty intervals */
        vector<vector<vector<long long>>> uncertainties = computeUncertaintyIntervals(signalsReal, eps, del);

        /* compute the canonical segmentation */
        vector<long long> segmentation = computeCanonicalSegmentation(signalsReal, uncertainties, d);
        numSegments = segmentation.size() - 1;

        /* compute the value expressions */
        vector<vector<set<string>>> valExprs = computeValueExpressions(signalsReal, uncertainties, segmentation);
        cout << valExprs[0][10].size() << endl;
        vector<set<string>> vex = asyncProdStrDiffSqr(valExprs[0], valExprs[3]);
        cout << vex[10].size() << endl;
        vector<set<string>> vey = asyncProdStrDiffSqr(valExprs[1], valExprs[4]);
        cout << vey[10].size() << endl;
        vector<set<string>> vez = asyncProdStrDiffSqr(valExprs[2], valExprs[5]);
        cout << vez[10].size() << endl;
        vector<set<string>> vexy = asyncProdStrSum(vex, vey);
        cout << vexy[10].size() << endl;
        vector<set<string>> vexyz = asyncProdStrSum(vexy, vez);
        // skip the square root since the distance parameter is 0
        valExprs = {vexyz};

        /* translate signals to atomic propositions */
        //vector<vector<vector<bitset<SIZE>>>> aps = convertSignalsToAtomicPropositions(valExprs);

        /* evaluate the formula */
        // test = bitsetConjunction(aps[0], aps[1]);
        // test = bitsetEventually(test);
        // test = bitsetEventually(bitsetNegation(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1])))); // this is faster
        // test = bitsetAlways(bitsetNegation(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1])))); // this is faster
        ////test = bitsetNegation(bitsetAlways(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1]))));
        //test = bitsetUntilStrict(aps[0], aps[1]);
        // test = bitsetBoundedEventually(aps[0], segmentation, a, b, leftClosed, rightClosed);

        endtime = chrono::system_clock::now();
        chrono::duration<double, milli> totalTime = endtime - starttime;

        cout << d << " " << eps << " " << del << " " << numSegments << " " << (totalTime.count() / REP) / 1000 << " " << test[0][0].any() << " " << test[0][1].any() << endl;
    }

    return 0;
}