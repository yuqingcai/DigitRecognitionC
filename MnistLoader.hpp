#ifndef MNIST_LOADER_HPP
#define MNIST_LOADER_HPP

#include <string>

int MnistToFile(const char* srcFileName, const char* destFileName);
void MnistToMemory(const char* srcFileName, unsigned char** ptr, size_t* length);

#endif // MNIST_LOADER_HPP