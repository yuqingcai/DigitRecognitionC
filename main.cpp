#include <cstdlib>
#include <cstdio>
#include <vector>
#include "MnistLoader.hpp"
#include "Image.hpp"
#include "Helper.hpp"
#include "Network.hpp"
#include "Network2.hpp"

using namespace std;

vector<Image*>* CreateImages(const char* imageFileName,
    const char* labelFileName)
{
    vector<Image*>* images = nullptr;

    unsigned char* imageBytes = nullptr;
    size_t imageSize = 0;

    unsigned char* labelBytes = nullptr;
    size_t labelSize = 0;

    int magicNumber = 0;
    int imageCount  = 0;
    int imageWidth  = 0;
    int imageHeight = 0;

    unsigned char* imagePtr = nullptr;
    unsigned char* labelPtr = nullptr;

    MnistToMemory(imageFileName, &imageBytes, &imageSize);
    if (!imageBytes) {
        printf("load images error: %s", imageFileName);
        goto exit;
    }

    MnistToMemory(labelFileName, &labelBytes, &labelSize);
    if (!labelBytes) {
        printf("load label error: %s", labelFileName);
        goto exit;
    }

    magicNumber = int32_swap(((int*)imageBytes)[0]);
    imageCount  = int32_swap(((int*)imageBytes)[1]);
    imageWidth  = int32_swap(((int*)imageBytes)[2]);
    imageHeight = int32_swap(((int*)imageBytes)[3]);
    

    printf("load images and labels from: %s, %s\n", 
        imageFileName, labelFileName);
    printf("magic number:%d\n", magicNumber);
    printf("image count:%d\n", imageCount);
    printf("image width:%d\n", imageWidth);
    printf("image height:%d\n", imageHeight);
    printf("\n");

    // 4 bytes Magic Number
    // 4 bytes Image Count
    // 4 bytes Image Width
    // 4 bytes Image Height
    imagePtr = &imageBytes[16];

    // 4 bytes Magic Number
    // 4 bytes Image Count
    labelPtr = &labelBytes[8];

    images = new vector<Image*>();
    for (int k = 0; k < imageCount; k ++) {
        Image* image = new Image(imageWidth, imageHeight);
        image->setBytes(&imagePtr[k*(imageWidth*imageHeight)]);
        image->setLabel(labelPtr[k]);
        images->push_back(image);
    }

exit:
    if (labelBytes) {
        free(labelBytes);
    }

    if (imageBytes) {
        free(imageBytes);
    }

    return images;
}

void DeleteImages(vector<Image*>* images)
{
    for (vector<Image*>::iterator it = images->begin(); 
        it != images->end(); it ++) {
        delete *it;
    }
    
    images->clear();
    delete images;
}

int main()
{
    vector<Image*>* trainingImages = 
        CreateImages("dataset/train-images-idx3-ubyte.gz", 
            "dataset/train-labels-idx1-ubyte.gz");

    vector<Image*>* testImages = 
        CreateImages("dataset/t10k-images-idx3-ubyte.gz", 
            "dataset/t10k-labels-idx1-ubyte.gz");
    
//    Network net = Network(vector<int>{784, 30, 10});
//    net.SGD(*trainingImages, 30, 10, 3.0, *testImages);
    
    Network2 net = Network2(vector<int>{784, 30, 10});
    net.SGD(*trainingImages, 30, 10, 0.5, *testImages);
    
    DeleteImages(testImages);
    DeleteImages(trainingImages);
    return 0;
}
