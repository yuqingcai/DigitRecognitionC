
#include <cstdlib>
#include <cstdio>
#include <cassert>
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
    if (_weights) {
        freeWeights(_weights);
    }
    
    if (_biases) {
        freeBiases(_biases);
    }
}

vector<vector<float>*>* Network::createBiases(float initValue, bool random)
{
    default_random_engine generator;
    normal_distribution<float> distribution(0.0, 1.0);
    
    vector<vector<float>*>* biases = new vector<vector<float>*>();

    for (size_t i = 1; i < _layers.size(); i ++) {

        vector<float>* b = new vector<float>();

        for (int j = 0; j < _layers[i]; j ++) {
            if (random) {
                b->push_back(distribution(generator));
            }
            else {
                b->push_back(initValue);
            }
        }
        biases->push_back(b);
    }

    return biases;
}

void Network::copyBiases(vector<vector<float>*>* dest,
    vector<vector<float> *>* src)
{
    for (size_t i = 0; i < src->size(); i ++)
    {
        for (size_t j = 0; j < src->at(i)->size(); j ++)
        {
            dest->at(i)->at(j) = src->at(i)->at(j);
        }
    }
}

void Network::exportBiases(vector<vector<float>*>* biases)
{
    FILE* fp = std::fopen("biases.json", "w");
    
    fprintf(fp, "{\"bias\" : [");
    
    for (size_t i = 0; i < biases->size(); i ++)
    {
        fprintf(fp, "[");
        
        for (size_t j = 0; j < biases->at(i)->size(); j ++)
        {
            fprintf(fp, "%0.4f", biases->at(i)->at(j));
            if (j < biases->at(i)->size() - 1) {
                fprintf(fp, ",");
            }
        }
        
        fprintf(fp, "]");
        if (i < biases->size() - 1) {
            fprintf(fp, ",");
        }
    }
    
    fprintf(fp, "]}");
    fclose(fp);
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
    default_random_engine generator;
    normal_distribution<float> distribution(0.0, 1.0);

    vector<vector<vector<float>*>*>* weights =
        new vector<vector<vector<float>*>*>();

    for (size_t i = 0; i < _layers.size() - 1; i ++) {

        vector<vector<float>*>* w = new vector<vector<float>*>();

        for (int j = 0; j < _layers[i]; j ++) {

            vector<float>* ww = new vector<float>();

            for (int k = 0; k < _layers[i+1]; k ++) {
                if (random) {
                    ww->push_back(distribution(generator));
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

void Network::copyWeights(vector<vector<vector<float>*>*>* dest,
    vector<vector<vector<float>*>*>* src)
{
    for (size_t i = 0; i < src->size(); i++)
    {
        for (size_t j = 0; j < src->at(i)->size(); j++)
        {
            for (size_t k = 0; k < src->at(i)->at(j)->size(); k++) {
                dest->at(i)->at(j)->at(k) = src->at(i)->at(j)->at(k);
            }
        }
    }
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

void Network::exportWeights(vector<vector<vector<float>*>*>* weights)
{
    
    FILE* fp = std::fopen("weights.json", "w");
    
    fprintf(fp, "{\"weights\" : [");
    
    for (size_t i = 0; i < weights->size(); i++)
    {
        
        fprintf(fp, "[");
        
        for (size_t j = 0; j < weights->at(i)->size(); j++)
        {
            
            fprintf(fp, "[");
            
            for (size_t k = 0; k < weights->at(i)->at(j)->size(); k++) {
                fprintf(fp, "%0.4f", weights->at(i)->at(j)->at(k));
                
                if (k < weights->at(i)->at(j)->size() - 1) {
                    fprintf(fp, ",");
                }
            }
            
            fprintf(fp, "]");
            if (j < weights->at(i)->size() - 1) {
                fprintf(fp, ",");
            }
        }
        
        fprintf(fp, "]");
        if (i < weights->size()-1) {
            fprintf(fp, ",");
        }
    }
    
    
    fprintf(fp, "]}");
    fclose(fp);
}

void Network::SGD(vector<Image*>& trainingImages,
    int epochs,
    int miniBatchSize,
    float eta,
    vector<Image*>& testImages)
{
    vector<vector<float> *>* bestBiases = createBiases(0.0, false);
    vector<vector<vector<float> *> *>* bestWeights = createWeights(0.0, false);

    size_t n = trainingImages.size();
    _bestMatchCount = 0;
    
    for (int j = 0; j < epochs; j++)
    {
        shuffleImages(trainingImages);
        
        vector<vector<Image*>> miniBatches;
        
        for (size_t k = 0; k < n; k += miniBatchSize) {
            vector<Image*> miniBatch;
            for (int i = 0; i < miniBatchSize; i ++) {
                miniBatch.push_back(trainingImages[k+i]);
            }
            miniBatches.push_back(miniBatch);
        }
        
        for (vector<Image*> miniBatch : miniBatches) {
            updateMiniBatch(miniBatch, eta);
        }

        int matchCount = evaluate(testImages);
        printf("Epoch %d : %d / %ld\n",
           j, matchCount, testImages.size());

        if (matchCount > _bestMatchCount) {
            _bestMatchCount = matchCount;
            copyWeights(bestWeights, _weights);
            copyBiases(bestBiases, _biases);
        }
    }

    exportBiases(bestBiases);
    exportWeights(bestWeights);

    freeWeights(bestWeights);
    freeBiases(bestBiases);
}

void Network::updateMiniBatch(vector<Image*>& miniBatch, float eta)
{
    vector<vector<float>*>* nablaB = createBiases(0.0, false);
    vector<vector<vector<float>*>*>* nablaW = createWeights(0.0, false);
    
    for (Image* image : miniBatch) {
        
        tuple<
            vector<vector<float>*>*,
            vector<vector<vector<float>*>*>*
        > delta = backprop(image->getBytes(), image->getLabel());

        vector<vector<float>*>* deltaNablaB = get<0>(delta);
        vector<vector<vector<float>*>*>* deltaNablaW = get<1>(delta);
        
        for (size_t l = 0; l < nablaB->size(); l ++) {
            for (size_t j = 0; j < nablaB->at(l)->size(); j ++) {
                nablaB->at(l)->at(j) += deltaNablaB->at(l)->at(j);
            }
        }
        
        for (size_t l = 0; l < nablaW->size(); l ++) {
            for (size_t k = 0; k < nablaW->at(l)->size(); k ++) {
                for (size_t j = 0; j < nablaW->at(l)->at(0)->size(); j ++) {
                    nablaW->at(l)->at(k)->at(j) += deltaNablaW->at(l)->at(k)->at(j);
                }
            }
        }
                
        freeBiases(deltaNablaB);
        freeWeights(deltaNablaW);
        
    }
    
    
    for (size_t l = 0; l < nablaB->size(); l ++) {
        for (size_t j = 0; j < nablaB->at(l)->size(); j ++) {
            _biases->at(l)->at(j) -= (eta/miniBatch.size()) *
                nablaB->at(l)->at(j);
        }
    }
    
    for (size_t l = 0; l < nablaW->size(); l ++) {
        for (size_t k = 0; k < nablaW->at(l)->size(); k ++) {
            for (size_t j = 0; j < nablaW->at(l)->at(0)->size(); j ++) {
                _weights->at(l)->at(k)->at(j) -= (eta/miniBatch.size()) *
                    nablaW->at(l)->at(k)->at(j);
            }
        }
    }
    
    freeWeights(nablaW);
    freeBiases(nablaB);
    
}

tuple<vector<vector<float>*>*,
    vector<vector<vector<float>*>*>*>
Network::backprop(vector<float>x, vector<float>y)
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
    vector<float> nablaC = costDerivative(activations.back(), y); // ∇C
    vector<float> sigmoidPrimes = sigmoidPrime(zs.back()); // σ'(zL)
    vector<float> delta = HadamardProduct(nablaC, sigmoidPrimes); // ẟL = ∇C ⊙ σ'(zL)
    
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
    
    // ∂C/∂b = ẟ
    for (size_t l = 0; l < nablaB->size(); l ++) {
        for (size_t j = 0; j < nablaB->at(l)->size(); j ++) {
            nablaB->at(l)->at(j) = deltas[l][j];
        }
    }
    
    // ∂C/∂w[l][k][j] = a[l-1][k]*ẟ[l][j]
    // The subscripts are different from Michael Nielsen's book
    //
    for (size_t l = 0; l < nablaW->size(); l ++) {
        for (size_t k = 0; k < nablaW->at(l)->size(); k ++) {
            for (size_t j = 0; j < nablaW->at(l)->at(0)->size(); j ++) {
                nablaW->at(l)->at(k)->at(j) = activations[l][k] * deltas[l][j];
            }
        }
    }
        
    return { nablaB, nablaW };
}

int Network::maxIndexInVector(vector<float> v)
{
    int j = 0;
    for (size_t i = 1; i < v.size(); i ++) {
        if (v[j] < v[i])
            j = (int)i;
    }
    return j;
}

int Network::evaluate(vector<Image*>& images)
{
    int c = 0;
    for (size_t i = 0; i < images.size(); i ++) {
        vector<float> x = feedforward(images[i]->getBytes());
        vector<float> y = images[i]->getLabel();
        int a = maxIndexInVector(x);
        int b = maxIndexInVector(y);
        if (a == b) {
            c ++;
        }
    }
    
    return c;
}

void Network::dumpBiases(vector<vector<float>*>* biases)
{
    for (size_t i = 0; i < biases->size(); i ++) {
        vector<float>* b = biases->at(i);
        printf("[");
        for (size_t j = 0; j < b->size(); j ++) {
            printf("%0.4f, ", b->at(j));
        }
        printf("]");
    }
    printf("\n");
}

void Network::dumpWeights(vector<vector<vector<float>*>*>* weights)
{
    for (size_t i = 0; i < weights->size(); i ++) {
        vector<vector<float>*>* w = weights->at(i);
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
