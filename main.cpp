#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat imgOriginal, imgGray, imgBlur, imgCanny, imgThreshold, imgDilation, imgErode;

Mat preProcessing(Mat img) {
    
    cvtColor(img, imgGray, COLOR_BGR2GRAY); //converting the image to grayscale
    GaussianBlur(imgGray, imgBlur, Size(3,3), 3, 0); //blurring the image using Gaussian function
    Canny(imgBlur, imgCanny, 25, 75); //outlines the edges found in the photo, thus finding the edges of the paper to be scanned as well as any other edges in the photo
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3,3)); //defining the kernel for the dilation
    dilate(imgCanny, imgDilation, kernel); //dilating the image thus making the foreground element (the edges) more apparent
    
    return imgDilation; //returning this processed image
    
}

int main() {

    string path = "Assets/paper.jpg"; //defining the path to the test image
    imgOriginal = imread(path); //declaring the image (Mat is the object that opencv uses)
    
    resize(imgOriginal, imgOriginal, Size(), 0.5, 0.5); //downsizing the image
    
    //Pre-Processing the image to be able to identify the contours (borders of the sheet).
    imgThreshold = preProcessing(imgOriginal);
    
    imshow("Original Image", imgOriginal); //displaying the image
    imshow("Threshold Image", imgThreshold); //displaying the threshold image
    
    waitKey(0); //allows it to display infintely

    return 0;

}

