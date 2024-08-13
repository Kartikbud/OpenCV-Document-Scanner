#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {

    string path = "Assets/paper.jpg"; //defining the path to the image
    Mat img = imread(path); //decalring the image (Mat is the object that opencv uses)
    imshow("image", img); //displaying the image
    waitKey(0); //allows it to display infintely

    return 0;

}

