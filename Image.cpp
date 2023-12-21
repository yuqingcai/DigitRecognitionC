#include <iostream>
#include "Image.hpp"

Image::Image(int width, int height)
    :
    _width(width),
    _height(height)
{
    // std::cout << "Image Conscturctor" << std::endl;
}

Image::~Image()
{
    // std::cout << "Image Deconstructor" << std::endl;
    _bytes.clear();
    _label.clear();
}

void Image::setBytes(unsigned char* src)
{
    _bytes.clear();
    for (int i = 0; i < _width * _height; i ++) {
        _bytes.push_back(src[i]/255.0);
    }
}

int Image::getWidth()
{
    return _width;
}

int Image::getHeight()
{
    return _height;
}

void Image::display()
{
    char gray[] = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";

    printf("label: [");
    for (size_t i = 0; i < _label.size(); i ++) {
        printf("%1.0f, ", _label[i]);
    }
    printf("]\n");

    for (int y = 0; y < _height; y ++) {
        for (int x = 0; x < _width; x ++) {
            unsigned char i = (_bytes[y*_width + x] * 255);
            if (i >= strlen(gray))
                i = strlen(gray)-1;
            printf("%c ", gray[i]);
        }
        printf("\n");
    }
    printf("\n");
}

void Image::setLabel(unsigned char label)
{
    _label.clear();
    for (int i = 0; i < 10; i ++) {
        if (i == label)
            _label.push_back(1.0);
        else
            _label.push_back(0.0);
    }
}

vector<float>& Image::getBytes()
{
    return _bytes;
}

vector<float>& Image::getLabel()
{
    return _label;
}