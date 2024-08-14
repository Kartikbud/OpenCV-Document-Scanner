#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat imgOriginal, imgGray, imgBlur, imgCanny, imgThreshold, imgDilation, imgWarp;

vector<Point> initialPoints, docPoints;

float width = 420, height = 596; //Values that represent the ratio between the width and height of the actual sheet of paper. These values work best with roughly A4 paper.

Mat preProcessing(Mat img) {
    
    cvtColor(img, imgGray, COLOR_BGR2GRAY); //converting the image to grayscale
    GaussianBlur(imgGray, imgBlur, Size(3,3), 3, 0); //blurring the image using Gaussian function
    Canny(imgBlur, imgCanny, 25, 75); //outlines the edges found in the photo, thus finding the edges of the paper to be scanned as well as any other edges in the photo
    Mat kernel = getStructuringElement(MORPH_RECT, Size(3,3)); //defining the kernel for the dilation
    dilate(imgCanny, imgDilation, kernel); //dilating the image thus making the foreground element (the edges) more apparent
    
    return imgDilation; //returning this processed image
    
}

vector<Point> getContours(Mat image){
    
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    
    findContours(image, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    
    vector<vector<Point>> conPoly(contours.size());
    vector<Rect> boundRect(contours.size());
    
    vector<Point> biggest;
    
    int maxArea = 0;
    
    for (int i = 0; i < contours.size(); i++){
        
        int area = contourArea(contours[i]);
        cout << area << endl;
        
        string objectType;
        
        if (area > 1000) {
            
            float perimeter = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02*perimeter, true);
            
            if (area > maxArea && conPoly[i].size() == 4) {
                
                //drawContours(imgOriginal, conPoly, i, Scalar(255, 0, 255), 5); uncomment this to show the actual edges registered by the function
                biggest = {conPoly[i][0], conPoly[i][1], conPoly[i][2], conPoly[i][3]};
                maxArea = area;
                
            }
            
        }
        
    }
    
    return biggest;
    
}

void drawPoints (vector<Point> points, Scalar color) { //function that draws labels to the points for demonstration purposes
    
    for (int i = 0; i < points.size(); i++) {
        
        circle(imgOriginal, points[i], 10, color, FILLED);
        putText(imgOriginal, to_string(i), points[i], FONT_HERSHEY_PLAIN, 4, color, 4);
        
    }
    
}

vector<Point> reorder(vector<Point> points) { //function takes the initial points and reorders them to ensure that the top left is labelled 0, top right is 1, bottom left is 2, bottom right is 3. This ensures that the warp function used later on works everytime.
    
    vector<Point> newPoints;
    vector<int> sumPoints, subPoints;
    
    for (int i = 0; i < 4; i++) {
        
        sumPoints.push_back(points[i].x + points[i].y); //this function holds the values of the x and y values of each point being added
        subPoints.push_back(points[i].x - points[i].y); //this function holds the values of the y values being subtracted from the x values of each point
        
    }
    
    newPoints.push_back(points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); //index 0 because the smallest value in the sumPoints list will be the top left corner
    newPoints.push_back(points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); //index 1 because the value with the larger difference between x and y indicates it being in the top right corner
    newPoints.push_back(points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); //index 2 because the value with the smaller difference between x and y indicates it being in the bottom left corner
    newPoints.push_back(points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); //index 3 because the largest value in the sumPoints list will be the bottom right corner
    
    
    return newPoints;
    
}

Mat getWarp(Mat img, vector<Point> points, float width, float height){
    
    Point2f source[4] = {points[0], points[1], points[2], points[3]}; //the corner points on the original image
    Point2f destination[4] = {{0.0f, 0.0f}, {width, 0.0f}, {0.0f, height}, {width, height}}; //the new coordinates for each corner on the new warped image
    
    Mat matrix = getPerspectiveTransform(source, destination); //calculating the matrix to be used for creating the warped image
    warpPerspective(img, imgWarp, matrix, Point(width, height)); //warping the image
    
    return imgWarp;
    
}

int main() {

    string path = "Assets/test-page.jpeg"; //defining the path to the test image
    imgOriginal = imread(path); //declaring the image (Mat is the object that opencv uses)
    
    //resize(imgOriginal, imgOriginal, Size(), 0.5, 0.5); //downsizing the image
    
    //Pre-Processing the image to be able to identify the contours (borders of the sheet).
    imgThreshold = preProcessing(imgOriginal);
    
    //Getting the contours of the page
    initialPoints = getContours(imgThreshold); //unordered corner points
    
    //Reordering
    docPoints = reorder(initialPoints);
    //drawPoints(docPoints, Scalar(0, 255, 0)); uncomment this to see the labelling of the corner points after being reordered
    
    //Warping the image to be a top down view of the page
    imgWarp = getWarp(imgOriginal, docPoints, width, height);
    
    imshow("Original Image", imgOriginal); //displaying the image
    //imshow("Threshold Image", imgThreshold); //displaying the threshold image, can uncomment to display the image after preprocessing
    imshow("Scanned Image", imgWarp); //displaying the warped image
    
    waitKey(0); //allows it to display infintely

    return 0;

}

