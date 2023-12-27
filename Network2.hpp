#ifndef NETWORK2_HPP
#define NETWORK2_HPP
#include "Network.hpp"

using namespace std;

class Network2: public Network
{
public:
    Network2(vector<int> layers);
    virtual ~Network2();
    
protected:
    
    virtual tuple< vector<vector<float>*>*, vector<vector<vector<float>*>*>* >
        backprop(vector<float>x, vector<float>y);
    
    vector<float> crossEntropyDelta(vector<float> a, vector<float> y);
    
};

#endif // NETWORK2_HPP

