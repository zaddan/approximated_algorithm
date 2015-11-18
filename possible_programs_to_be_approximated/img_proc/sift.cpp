#include <iostream>
//#include <opencv2/opencv.hpp>
//#include <opencv2/nonfree/nonfree.hpp>
//#include "opencv2/legacy/legacy.hpp"

#include "opencv.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
//#include "opencv2/legacy/legacy.hpp"
using namespace cv;
using namespace std;
using namespace xfeatures2d;
int main(int argc, char** argv)
{
    //read images
    Mat img_1c=imread("img3.jpg");
    Mat img_2c=imread("img1.jpg");
    
    Mat img_1, img_2;
    //transform images into gray scale
    cvtColor( img_1c, img_1, CV_BGR2GRAY );
    cvtColor( img_2c, img_2, CV_BGR2GRAY );

    SIFT sift;
    //Ptr<SIFT> ptrsift = SIFT::create(50, 3, .2, 5, 10);  //works for imag1 and 2
    Ptr<SIFT> ptrsift = SIFT::create(15, 5, .1, 5, 10); 
    vector<KeyPoint> key_points_1, key_points_2;
    Mat detector;
    //do sift, find key points
    ptrsift->detect(img_1, key_points_1);
    ptrsift->detect(img_2, key_points_2);
    //sift(img_2, Mat(), key_points_2, detector);

    //PSiftDescriptorExtractor extractor;
    Ptr<SIFT> extractor = SIFT::create(); 
    
    Mat descriptors_1,descriptors_2;
    //compute descriptors
    extractor->compute(img_1,key_points_1,descriptors_1);
    extractor->compute(img_2,key_points_2,descriptors_2);
    cout<<descriptors_1;
    //use burte force method to match vectors
    BFMatcher matcher;
    vector<DMatch>matches;
    matcher.match(descriptors_1,descriptors_2,matches);

    //draw results
    Mat img_matches;
    drawMatches(img_1c,key_points_1,img_2c,key_points_2,matches,img_matches);
    imshow("sift_Matches",img_matches);
    waitKey(0);
    return 0;
}
