#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <tuple>
#include <map>
#include <set>
#include <bitset>
#include <algorithm>
#include <limits>
#include <cfloat>
using namespace std;

int main() {

    vector<int> N {1};
    vector<int> D {4, 8, 16, 32, 64, 128, 256, 512, 1024};

    for (const auto &n : N) { 
        for (const auto &d : D) {
            for (int i = 0; i < 200; i++) {
                        random_device rd;
                        mt19937 gen(rd());
                        uniform_int_distribution<> valDist(-100, 100);
                        
                        vector<int> signal;
                        signal.resize(d);
                        for (int j = 0; j < d; j++) {
                            signal[j] = valDist(gen);
                        }

                            ofstream sigdata;
                            string filename = "dataNew/" + to_string(d) + "_" + to_string(i) + ".txt";
                            sigdata.open(filename);

                            for (int k = 0; k < d; k++) {
                                sigdata << k << " " << signal[k] << endl;
                            }

                            sigdata.close();
            }
        }
    }

    return 0;
}

