#ifndef NETWORK_HPP
#define NETWORK_HPP
#include <vector>
#include <tuple>
#include "Image.hpp"

using namespace std;

class Network 
{
public:
    Network(vector<int> layers);
    virtual ~Network();
    
    void SGD(vector<Image*>& trainingImages,
        int epochs,
        int miniBatchSize,
        float eta,
        vector<Image*>& testImages);
    
    void dumpBiases(vector<vector<float>*>* biases);
    void dumpWeights(vector<vector<vector<float>*>*>* weights);
    void dumpVector(vector<float> v);

protected:
    
    void shuffleImages(vector<Image*>& images);
    float sigmoid(float z);
    vector<float> sigmoid(vector<float> z);
    float sigmoidPrime(float z);
    vector<float> sigmoidPrime(vector<float> z);
    void updateMiniBatch(vector<Image*>& miniBatch, float eta);
    int evaluate(vector<Image*>& images);
    vector<float> feedforward(vector<float> a);
    
    virtual tuple< vector<vector<float>*>*, vector<vector<vector<float>*>*>* >
        backprop(vector<float>x, vector<float>y);
        
    vector<float> costDerivative(vector<float> a, vector<float> y);
    vector<float> HadamardProduct(vector<float> a, vector<float> b);
    vector<vector<float>*>*
        createBiases(float initValue, bool random);
    void copyBiases(vector<vector<float> *>* dest,
        vector<vector<float> *> *src);
    void freeBiases(vector<vector<float>*>* biases);
    void exportBiases(vector<vector<float>*>* biases);
    
    vector<vector<vector<float>*>*>*
        createWeights(float initValue, bool random);
    void copyWeights(vector<vector<vector<float>*>*>* dest,
                     vector<vector<vector<float>*>*>* src);
    void freeWeights(vector<vector<vector<float>*>*>* weights);
    void exportWeights(vector<vector<vector<float>*>*>* weights);
    
    int maxIndexInVector(vector<float> v);
    
    vector<int> _layers;
    vector<vector<float>*>* _biases;
    vector<vector<vector<float>*>*>* _weights;
    
    int _bestMatchCount;
};

#endif // NETWORK_HPP

