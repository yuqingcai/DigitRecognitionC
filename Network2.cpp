
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <random>
#include "Network2.hpp"

Network2::Network2(vector<int> layers)
    :
    Network(layers)
{
}

Network2::~Network2()
{
}


tuple<vector<vector<float>*>*,
    vector<vector<vector<float>*>*>*>
Network2::backprop(vector<float>x, vector<float>y)
{
    vector<vector<float>*>* nablaB = createBiases(0.0, false);
    vector<vector<vector<float>*>*>* nablaW = createWeights(0.0, false);
    
    // feedforward
    vector<vector<float>> zs;
    vector<vector<float>> activations;
    
    activations.push_back(x);
        
    for (size_t l = 0; l < _layers.size()-1; l ++) {
        vector<float> av;
        vector<float> zv;

        for (int j = 0; j < _layers[l+1]; j ++) {
            float z = 0.0;
            for (int k = 0; k < _layers[l]; k ++) {
                z += x[k] * _weights->at(l)->at(k)->at(j);
            }
            z += _biases->at(l)->at(j);

            zv.push_back(z);
            av.push_back(sigmoid(z));
        }

        zs.push_back(zv);
        activations.push_back(av);

        x = av;
    }

    // backward pass
    vector<float> delta = crossEntropyDelta(activations.back(), y);
    vector<vector<float>> deltas;
    deltas.insert(deltas.begin(), delta);
    
    // l point to the second last Neural networks layer, and the last zs layer
    size_t l = _layers.size() - 2;
    
    while (l > 0) {
        vector<float> wdv;
        for (int k = 0; k < _layers[l]; k ++) {
            float t = 0.0;
            for (int j = 0; j < _layers[l+1]; j ++) {
                t += _weights->at(l)->at(k)->at(j) * delta[j];
            }
            wdv.push_back(t);
        }
        delta = HadamardProduct(wdv, sigmoidPrime(zs[l-1]));
        deltas.insert(deltas.begin(), delta);
        l --;
    }
    
    for (size_t l = 0; l < nablaB->size(); l ++) {
        for (size_t j = 0; j < nablaB->at(l)->size(); j ++) {
            nablaB->at(l)->at(j) = deltas[l][j];
        }
    }
    
    for (size_t l = 0; l < nablaW->size(); l ++) {
        for (size_t k = 0; k < nablaW->at(l)->size(); k ++) {
            for (size_t j = 0; j < nablaW->at(l)->at(0)->size(); j ++) {
                nablaW->at(l)->at(k)->at(j) = activations[l][k] * deltas[l][j];
            }
        }
    }
        
    return { nablaB, nablaW };
}

// cross-entropy cost function delta
vector<float> Network2::crossEntropyDelta(vector<float> a, vector<float> y)
{
    vector<float> delta;
    for (size_t i = 0; i < a.size(); i ++) {
        delta.push_back(a[i] - y[i]);
    }
    return delta;
}
