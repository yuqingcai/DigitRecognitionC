#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <vector>

using namespace std;

class Image 
{
public:
    Image(int width, int height);
    ~Image();
    void setBytes(unsigned char* src);
    int getWidth();
    int getHeight();
    void display();
    void setLabel(unsigned char label);
    vector<float>& getBytes();
    vector<float>& getLabel();
    
private:
    int _width;
    int _height;
    vector<float> _bytes;
    vector<float> _label;
};

#endif // IMAGE_HPP
