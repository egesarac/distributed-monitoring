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
    vector<long long> D{4000, 8000};
    vector<long long> EPS{1000, 2000, 4000};
    vector<long long> DEL{1, 1000, 2000};
    int n = 2;
    /*
    long long a = 0;
    long long b = 8000;
    bool leftClosed = true;
    bool rightClosed = false;
    */

    ofstream results;
    string filename = "results____.txt";
    results.open(filename);

    for (const auto &d : D)
    {
        for (const auto &eps : EPS)
        {
            if (d < eps)
            {
                continue;
            }

            for (const auto &del : DEL)
            {
                if (eps < del)
                {
                    continue;
                }

                for (int c = 0; c < 100; c++)
                {

                            cout << d << " " << c << " " << eps << " " << del << endl;

                    /* read signal data */
                    vector<vector<pair<long long, double>>> signalsReal(n);
                    signalsReal[0] = getData("dataNew/" + to_string(d / 1000) + "_" + to_string(c) + ".txt");
                    signalsReal[1] = getData("dataNew/" + to_string(d / 1000) + "_" + to_string(c + 100) + ".txt");

                    /* prepare the bit masks */
                    evenMask = generateBitmask(0);
                    oddMask = generateBitmask(1);

                    bool invalidSignal = false;
                    int numSegments;
                    vector<vector<bitset<SIZE>>> test;
                    chrono::time_point<chrono::system_clock> starttime;
                    chrono::time_point<chrono::system_clock> endtime; 
                    
                    starttime = chrono::system_clock::now();
                    for (int rep = 0; rep < REP; rep++)
                    {
                        /* read the signals as atomic propositions to improve performance */
                        vector<vector<pair<long long, double>>> signals = convertSignalsToBool(signalsReal);

                        /* compute the uncertainty intervals and check if the signal is valid*/
                        vector<vector<vector<long long>>> uncertainties = computeUncertaintyIntervals(signals, eps, del);
                        
                        for (int i = 0; i < n && !invalidSignal; i++)
                        {
                            for (int j = 1; j < signals[i].size(); j++)
                            {
                                if (uncertainties[i][j][0] > uncertainties[i][j][1])
                                {
                                    invalidSignal = true;
                                    break;
                                }
                            }
                        }

                        /* compute the canonical segmentation */
                        vector<long long> segmentation = computeCanonicalSegmentation(signals, uncertainties, d);
                        numSegments = segmentation.size() - 1;

                        /* compute the value expressions */
                        vector<vector<set<string>>> valExprs = computeValueExpressions(signals, uncertainties, segmentation);

                        /* translate signals to atomic propositions */
                        vector<vector<vector<bitset<SIZE>>>> aps = convertSignalsToAtomicPropositions(valExprs);

                        /* evaluate the formula */
                        // always(x1 and x2)
                        // test = bitsetAlways(bitsetConjunction(aps[0], aps[1]));

                        // always(x1 or x2)
                        // test = bitsetAlways(bitsetNegation(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1]))));

                        // eventually(x1 and x2)
                        // test = bitsetEventually(bitsetConjunction(aps[0], aps[1]));

                        // eventually(x1 or x2)
                        // test = bitsetEventually(bitsetNegation(bitsetConjunction(bitsetNegation(aps[0]), bitsetNegation(aps[1]))));

                        // x1 until x2
                        test = bitsetUntilStrict(aps[0], aps[1]);

                        vector<set<pair<string, string>>> prod = asyncProd(aps[0], aps[1]);
                        vector<set<string>> prodTest = prodUntilStrict(prod, false); // pass 'false' for strong until, 'true' for weak until

                        if (!isEqual(prodTest, test)) {
                            cout << "error: " << d << " " << c << " " << eps << " " << del << endl;
                            int x = 0;
                        }
                    }

                    endtime = chrono::system_clock::now();
                    chrono::duration<double, milli> totalTime = endtime - starttime;

                    string out = "";
                    if (test[0][0].any() && test[0][1].any())
                    {
                        out = "2";
                    }
                    else if (test[0][1].any())
                    {
                        out = "1";
                    }
                    else
                    {
                        out = "0";
                    }

                    string w = to_string(d / 1000) + " " + to_string(eps / 1000) + " " + to_string(del / 1000) + " " + to_string(c) + " ";
                    if (invalidSignal)
                    {
                         w = w + "-" + " " + "-" + " " + "-";
                    }
                    else
                    {
                        w = w + to_string(numSegments) + " " + to_string((totalTime.count() / REP) / 1000) + " " + out;
                    }
                    //results << w << endl;
                    //]cout << w << endl;
                }
            }
        }
    }

    results.close();

    return 0;
}
