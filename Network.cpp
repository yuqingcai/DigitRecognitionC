
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <ctime>
#include <random>
#include "Network.hpp"

Network::Network(vector<int> layers)
    :
    _layers(layers)
{
    _biases = createBiases(0.0, true);
    _weights = createWeights(0.0, true);
}

Network::~Network()
{
    freeWeights(_weights);
    freeBiases(_biases);
}

vector<vector<float>*>* Network::createBiases(float initValue, bool random)
{
    srand((unsigned)time(NULL));

    vector<vector<float>*>* biases = new vector<vector<float>*>();

    for (size_t i = 1; i < _layers.size(); i ++) {

        vector<float>* b = new vector<float>();

        for (int j = 0; j < _layers[i]; j ++) {
            if (random) {
                b->push_back((float)rand()/RAND_MAX);
            }
            else {
                b->push_back(initValue);
            }
        }
        biases->push_back(b);
    }

    return biases;
}

void Network::freeBiases(vector<vector<float>*>* biases)
{
    for (size_t i = 0; i < biases->size(); i ++) {
        vector<float>* bias = biases->at(i);
        bias->clear();
        delete bias;
    }
    biases->clear();
    delete biases;
}

vector<vector<vector<float>*>*>* 
    Network::createWeights(float initValue, bool random)
{
    srand((unsigned)time(NULL));

    vector<vector<vector<float>*>*>* weights = 
        new vector<vector<vector<float>*>*>();

    for (size_t i = 0; i < _layers.size() - 1; i ++) {

        vector<vector<float>*>* w = new vector<vector<float>*>();  

        for (int j = 0; j < _layers[i]; j ++) {

            vector<float>* ww = new vector<float>();

            for (int k = 0; k < _layers[i+1]; k ++) {
                if (random) {
                    ww->push_back((float)rand()/RAND_MAX);
                }
                else {
                    ww->push_back(initValue);
                }


            }
            w->push_back(ww);
        }
        weights->push_back(w);
    }
    return weights;
}

void Network::freeWeights(vector<vector<vector<float>*>*>* weights)
{
    for (size_t i = 0; i < weights->size(); i ++) {
        vector<vector<float>*>* w = weights->at(i);

        for (size_t j = 0; j < w->size(); j ++) {

            vector<float>* ww = (*w)[j];
            ww->clear();
            delete ww;

        }

        w->clear();
        delete w;
    }
    weights->clear();
    delete weights;
}

void Network::SGD(vector<Image*>& trainingImages,
    int epochs, 
    int miniBatchSize, 
    float eta,
    vector<Image*>& testImages)
{
    int n = trainingImages.size();

    for (int j = 0; j < epochs; j ++) {

        shuffleImages(trainingImages);
        
        vector<vector<Image*>> miniBatches;
        for (int k = 0; k < n; k += miniBatchSize) {
            vector<Image*> miniBatch;
            for (int i = 0; i < miniBatchSize; i ++) {
                miniBatch.push_back(trainingImages[k+i]);
            }
            miniBatches.push_back(miniBatch);
        }

        for (vector<Image*> miniBatch : miniBatches) {
            updateMiniBatch(miniBatch, eta);
        }

        printf("Epoch %d : %d / %ld\n", 
            j, evaluate(testImages), testImages.size());
    }

}

void Network::updateMiniBatch(vector<Image*>& miniBatch, float eta)
{
    for (Image* image : miniBatch) {
        // image->display();
        tuple<
            vector<vector<float>*>*, 
            vector<vector<vector<float>*>*>*
        > delta = backprop(image->getBytes(), image->getLabel());

        vector<vector<float>*>* deltaNablaB = get<0>(delta);
        vector<vector<vector<float>*>*>* deltaNablaW = get<1>(delta);

        freeBiases(deltaNablaB);
        freeWeights(deltaNablaW);
    }
    (void)eta;
}

tuple<vector<vector<float>*>*, vector<vector<vector<float>*>*>*> 
    Network::backprop(vector<float>x, vector<float>y)
{
    vector<vector<float>*>* nablaB = createBiases(0.0, false);
    vector<vector<vector<float>*>*>* nablaW = createWeights(0.0, false);
    
    // feedforward
    vector<vector<float>> zs;
    vector<vector<float>> activations;
    activations.push_back(x);

    for (size_t l = 0; l < _weights->size(); l ++) {

        vector<float> activation;
        vector<float> zz;

        for (size_t j = 0; j < _weights->at(l)->at(0)->size(); j ++) {
    
            float z = 0.0;
            for (size_t k = 0; k < x.size(); k ++) {
                z += x[k] * _weights->at(l)->at(k)->at(j);
            }
            z += _biases->at(l)->at(j);
            zz.push_back(z);
            activation.push_back(sigmoid(z));
        }
        
        zs.push_back(zz);
        activations.push_back(activation);
        x = activation;
    }

    // backward pass
    vector<float> nablaC = costDerivative(activations.back(), y); // ∇C
    vector<float> sigmoidPrimes = sigmoidPrime(zs.back()); // σ'(zL)
    vector<float> delta = HadamardProduct(nablaC, sigmoidPrimes); // ẟL = ∇C ⊙ σ'(zL)

//    size_t l = _layers.size() - 2;
//
//    vector<vector<float>> deltas;
//    while (l >= 0) {
//
//        sigmoidPrimes = sigmoidPrime(zs[l]);
//        vector<float> s;
//        for (size_t k = 0; k < _weights->at(l)->at(0)->size(); k ++) {
//            float t = 0.0;
//            for (size_t j = 0; j < _weights->at(l)->at(0)->size(); j ++) {
//                t += _weights->at(l)->at(k)->at(j) * delta[j];
//            }
//            s.push_back(t);
//        }
//        s = HadamardProduct(s, sigmoidPrimes);
//        deltas.push_back(s);
//        delta = s;
//        l --;
//    }
    
    // l = nablaB->size()-1;
    // for (size_t i = 0; i < nablaB->at(l)->size(); i ++) {
    //     nablaB->at(l)->at(i) = delta[i];
    // }
    
    // l = nablaW->size()-1;
    // for (size_t k = 0; k < nablaW->at(l)->size(); k ++) {
    //     for (size_t j = 0; j < nablaW->at(l)->at(k)->size(); j ++) {
    //         nablaW->at(l)->at(k)->at(j) = activations[l-1][k]*delta[j];
    //     }
    // }

    return { nablaB, nablaW };
    
}

int Network::evaluate(vector<Image*>& images)
{
    (void)images;
    return 0;
}

void Network::dumpBias()
{
    for (size_t i = 0; i < _biases->size(); i ++) {
        vector<float>* b = _biases->at(i);
        printf("[");
        for (size_t j = 0; j < b->size(); j ++) {
            printf("%0.4f, ", b->at(j));
        }
        printf("]");
    }
    printf("\n");
}

void Network::dumpWeights()
{
    for (size_t i = 0; i < _weights->size(); i ++) {
        vector<vector<float>*>* w = _weights->at(i);
        printf("[");
        for (size_t j = 0; j < w->size(); j ++) {
            vector<float>* ww = w->at(j);
            printf("[");
            for (size_t k = 0; k < ww->size(); k ++) {
                printf("%0.4f, ", ww->at(k));
            }
            printf("]");
        }
        printf("]");
    }
    printf("\n");
}

void Network::dumpVector(vector<float> v)
{
    printf("[");
    for (size_t i = 0; i < v.size(); i ++) {
        printf("%0.4f, ", v[i]);
    }
    printf("]\n");
}

float Network::sigmoid(float z)
{
    return 1.0/(1.0 + exp(-z));
}

vector<float> Network::sigmoid(vector<float> z)
{
    vector<float> v;
    for (float _z : z) {
        v.push_back(sigmoid(_z));
    }
    return v;
}

float Network::sigmoidPrime(float z)
{   
    return sigmoid(z) * (1 - sigmoid(z));
}

vector<float> Network::sigmoidPrime(vector<float> z)
{
    vector<float> v;
    for (float _z : z) {
        v.push_back(sigmoidPrime(_z));
    }
    return v;
}

void Network::shuffleImages(vector<Image*>& images)
{
//    unsigned int seed = (unsigned int)chrono::system_clock::
//        now().time_since_epoch().count();
    
    shuffle(images.begin(), images.end(),
        default_random_engine((unsigned int)time(0)));
}

vector<float> Network::feedforward(vector<float> x)
{
    for (size_t l = 0; l < _weights->size(); l ++) {
        vector<float> a;
        for (size_t j = 0; j < _weights->at(l)->at(0)->size(); j ++) {
            float z = 0.0;
            for (size_t k = 0; k < x.size(); k ++) {
                z += x[k] * _weights->at(l)->at(k)->at(j);
            }
            z += _biases->at(l)->at(j);
            a.push_back(sigmoid(z));
        }
        x = a;
    }
    return x;
}


vector<float> Network::HadamardProduct(vector<float> a, vector<float> b)
{
    vector<float> v;
    for (size_t i = 0; i < a.size(); i ++) {
        v.push_back(a[i]*b[i]);
    }
    return v;
}

vector<float> Network::costDerivative(vector<float> a, vector<float> y)
{
    // Cost function is: C = 1/2 * Σ(y-a), the derivative is: ∂C/∂a = (a-y)
    vector<float> delta;
    for (size_t i = 0; i < a.size(); i ++) {
        delta.push_back(a[i] - y[i]);
    }
    return delta;
}
