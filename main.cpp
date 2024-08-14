#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

Mat imgOriginal, imgGray, imgBlur, imgCanny, imgThreshold, imgDilation, imgErode;

vector<Point> initialPoints, docPoints;

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
                
                drawContours(imgOriginal, conPoly, i, Scalar(255, 0, 255), 5);
                biggest = {conPoly[i][0], conPoly[i][1], conPoly[i][2], conPoly[i][3]};
                maxArea = area;
                
            }
            
        }
        
    }
    
    return biggest;
    
}

void drawPoints (vector<Point> points, Scalar color) {
    
    for (int i = 0; i < points.size(); i++) {
        
        circle(imgOriginal, points[i], 10, color, FILLED);
        putText(imgOriginal, to_string(i), points[i], FONT_HERSHEY_PLAIN, 4, color, 4);
        
    }
    
}

vector<Point> reorder(vector<Point> points) { //function takes the initial points and reorders them to ensure that the top left is labelled 0, top right is 1, bottom left is 2, bottom right is 3. This ensures that the warp function used later on works with any image
    
    vector<Point> newPoints;
    vector<int> sumPoints, subPoints;
    
    for (int i = 0; i < 3; i++) {
        
        sumPoints.push_back(points[i].x + points[i].y);
        subPoints.push_back(points[i].x - points[i].y);
        
    }
    
    newPoints.push_back(points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); //index 0
    newPoints.push_back(points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); //index 1
    newPoints.push_back(points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]); //index 2
    newPoints.push_back(points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]); //index 3
    
    
    return newPoints;
    
}

int main() {

    string path = "Assets/paper.jpg"; //defining the path to the test image
    imgOriginal = imread(path); //declaring the image (Mat is the object that opencv uses)
    
    resize(imgOriginal, imgOriginal, Size(), 0.5, 0.5); //downsizing the image
    
    //Pre-Processing the image to be able to identify the contours (borders of the sheet).
    imgThreshold = preProcessing(imgOriginal);
    
    //Getting the contours of the page
    initialPoints = getContours(imgThreshold);
    drawPoints(initialPoints, Scalar(0, 0, 255));
    
    //Reordering
    docPoints = reorder(initialPoints);
    drawPoints(docPoints, Scalar(0, 255, 0));
    
    imshow("Original Image", imgOriginal); //displaying the image
    imshow("Threshold Image", imgThreshold); //displaying the threshold image
    
    waitKey(0); //allows it to display infintely

    return 0;

}

