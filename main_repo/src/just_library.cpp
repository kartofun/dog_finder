#include "opencv2/opencv.hpp"
#include "opencv2/dnn.hpp"
//#include <filesystem>
#include <fstream>
#include <string>
#include <pybind11/pybind11.h>

using namespace cv;
using namespace std;

#define TRAIN_DATA "cat_dog_train_data.yml"

void trainNN()
{
    vector<String> fn;

    glob("images/*.jpg", fn, true);
    std::string modelTxt = "squeezenet_v1.1.prototxt";
    std::string modelBin = "squeezenet_v1.1.caffemodel";

    dnn::Net net = dnn::readNetFromCaffe(modelTxt, modelBin);
    cv::Size inputImgSize = cv::Size(227, 227); // model was trained with this size

    Mat_<int> layers(4, 1);
    layers << 1000, 400, 100, 2; // the sqeezenet pool10 layer has 1000 neurons

    Ptr<ml::ANN_MLP> nn = ml::ANN_MLP::create();
    nn->setLayerSizes(layers);
    nn->setTrainMethod(ml::ANN_MLP::BACKPROP, 0.0001);
    nn->setActivationFunction(ml::ANN_MLP::SIGMOID_SYM);
    nn->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER+TermCriteria::EPS, 300, 0.0001));

    Mat train, test;
    Mat labels(1199, 2, CV_32F, 0.f); // 1399 - 200 test images

    for (size_t i=0; i<fn.size(); i++) {
        Mat img = imread(fn[i]);
        net.setInput(dnn::blobFromImage(img, 1, inputImgSize, Scalar::all(127), false));
        Mat blob = net.forward("pool10");
        Mat f = blob.reshape(1,1).clone(); // now our feature has 1000 numbers

        if (i<100) {
            // test cat
            test.push_back(f);
        } else
        if (i>=100 && i<700) {
            // train cat
            train.push_back(f);
            labels.at<float>(i-100,0) = 1; // one-hot encoded labels for our ann
        } else
        if (i>=700 && i<800) {
            // test dog
            test.push_back(f);
        } else {
            // train dog
            train.push_back(f);
            labels.at<float>(i-200,1) = 1;
        }
        cout << i << "\r"; // "machine learning should come with a statusbar." ;)
    }

    cout << "train.size(): " << train.size() << " " << "labels.size(): " << labels.size() << " " << "test.size(): " << test.size() << endl;
    nn->train(train, 0, labels); // yes, that'll take a few minutes ..
    nn->save(TRAIN_DATA);

    Mat result;
    nn->predict(test,result);
    
    float correct_cat = 0;
    float correct_dog = 0;
    for (int i=0; i<100; i++) // 1st hundred testexamples were cats
        correct_cat += result.at<float>(i,0) > result.at<float>(i,1); // 0, true cat
    for (int i=100; i<200; i++) // 2nd hundred were dogs
        correct_dog += result.at<float>(i,1) > result.at<float>(i,0); // 1, true dog;
    float accuracy = (correct_cat + correct_dog) / 200;

    cout << "Correct Cat: " << correct_cat << " " << "Correct Dog: " << correct_dog << " : " << "Accuracy: " << accuracy << endl;
    cout << endl;

}

// std::string predict(std::string imgPath)
std::string predict(char* buffer, int bufferLength)
{
    std::ifstream file;
    file.open(TRAIN_DATA);
    file.close();
    if (file)
    {
        cout << "Train data is exist: " << TRAIN_DATA << endl;
    }
    else {
        
        cout<<"Train data is not present. Start train."; 
        trainNN();
    }

    Mat_<int> layers(4, 1);
    layers << 1000, 400, 100, 2; // the sqeezenet pool10 layer has 1000 neurons

    // Ptr<ml::ANN_MLP> nn = ml::ANN_MLP::create();
    Ptr<ml::ANN_MLP> nn = ml::ANN_MLP::load(TRAIN_DATA);
    // nn->setLayerSizes(layers);
    // nn->setTrainMethod(ml::ANN_MLP::BACKPROP, 0.0001);
    // nn->setActivationFunction(ml::ANN_MLP::SIGMOID_SYM);
    // nn->setTermCriteria(TermCriteria(TermCriteria::MAX_ITER+TermCriteria::EPS, 300, 0.0001));


    // Ptr<TrainData> tdata = TrainData::loadFromCSV(original_path, 0);

    // nn->load(TRAIN_DATA);

    std::string modelTxt = "../squeezenet_v1.1.prototxt";
    std::string modelBin = "../squeezenet_v1.1.caffemodel";

    dnn::Net net = dnn::readNetFromCaffe(modelTxt, modelBin);
    cv::Size inputImgSize = cv::Size(227, 227); // model was trained with this size

    // std::string test_str = imgPath;

    // cv::Mat predictMat=cv::imread(test_str);
    cv::Mat predictMat = cv::imdecode(cv::Mat(1, bufferLength, CV_8UC1, buffer), IMREAD_UNCHANGED);
    net.setInput(dnn::blobFromImage(predictMat, 1, inputImgSize, Scalar::all(127), false));
    Mat blob = net.forward("pool10");
    Mat f = blob.reshape(1,1).clone();

    float res = nn->predict(f);

    if (res == 0) {
        return "cat";
    }
    if (res == 1) {
        return "dog";
    }

    return "undefined";
}

// int main(int argc, char** argv)
// {
    
//     std::string res = predict("test_img4.jpg");

//     cout << "res: " << res << endl;

//     return 0;
// }

namespace py = pybind11;

PYBIND11_MODULE(just_library, m) {
    m.def("trainNN", &trainNN); // добавили функцию в модуль
    m.def("predict", &predict);
}