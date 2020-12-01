#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
using namespace cv;
using namespace std;

double angle( cv::Point pt1, cv::Point pt2, cv::Point pt0 ) {
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return ( dx1 * dx2 + dy1 * dy2 ) / sqrt( ( dx1 * dx1 + dy1 * dy1 ) * ( dx2 * dx2 + dy2 * dy2 ) + 1e-10 );
}

int main() {
    int h1 = 0, s1 = 80, v1 = 0, h2 = 255, s2 = 255, v2 = 255;
    namedWindow("oknoHSV",WINDOW_AUTOSIZE);
    createTrackbar("h - min","oknoHSV",&h1,255);
    createTrackbar("s - min","oknoHSV",&s1,255);
    createTrackbar("v - min","oknoHSV",&v1,255);
    createTrackbar("h - max","oknoHSV",&h2,255);
    createTrackbar("s - max","oknoHSV",&s2,255);
    createTrackbar("v - max","oknoHSV",&v2,255);


    int canny_a = 30, canny_b = 45;
    namedWindow("prostokąty", WINDOW_AUTOSIZE);
    createTrackbar("A","prostokąty", &canny_a,255);
    createTrackbar("B","prostokąty", &canny_b,255);
    VideoCapture camera(0);
    while (waitKey(1) != 27) {

        Mat frame, frame0, frameBw, frameCanny;
        camera >> frame;
        flip(frame,frame,1);
        frame0 = frame.clone();
        cvtColor(frame,frameBw,COLOR_BGR2GRAY);

        Canny(frameBw, frameCanny, canny_a,canny_b,3);
        static auto ellipse = getStructuringElement(MORPH_ELLIPSE, Size(5,5) );
        morphologyEx(frameBw, frameBw, MORPH_CLOSE, ellipse);
        morphologyEx(frameBw, frameBw, MORPH_OPEN, ellipse);

        Canny(frameBw, frameCanny, canny_a,canny_b,3);
        static auto ellipse_33 = getStructuringElement(MORPH_ELLIPSE, Size(6,6));
        morphologyEx(frameCanny, frameCanny,MORPH_DILATE,ellipse_33);

        imshow("bw_Canny2_dil", frameCanny);
        vector<vector<Point>> contours;
        vector<vector<Point>> contours_inRANGE;
        vector<vector<Point>> contours_4;
        findContours(frameCanny, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
        for(int i = 0; i < contours.size(); i++) {
            approxPolyDP(contours[i],contours[i],10,true);
            if(contours[i].size() == 4) {
                double amax = 0;
                for(int j = 0; j < 4; j++) {
                    double a = fabs(angle(contours[i][j], contours[i][(j+1)%4], contours[i][(j+1)%4]));
                    amax = max(a,amax);
                }
                if(amax < 0.4)
                    contours_4.push_back(contours[i]);
            }
        }

        if(contours_4.size() > 0)
        {
            sort(contours_4.begin(),contours_4.end(),
                 [](auto &a, auto &b) {
                     return contourArea(a,false) > contourArea(b,false);
                 });
            drawContours(frame, contours_4,0,Scalar(0,0,255));
            Mat dstMat(Size(250,300), CV_8UC3);
            vector<Point2f> src = {{0,0}, {static_cast<float>(dstMat.cols),0}, {static_cast<float>(dstMat.cols), static_cast<float>(dstMat.rows)}, {0, static_cast<float>(dstMat.rows)}};
            vector<Point2f> dst;
            for(auto p: contours_4[0])
                dst.push_back(Point2f(p.x,p.y));
            auto wrap_mtx = getPerspectiveTransform(dst, src);
            warpPerspective(frame0, dstMat, wrap_mtx, Size(dstMat.cols, dstMat.rows));
            Mat dstMat2;

            cvtColor(dstMat, dstMat2, COLOR_BGR2HSV);
            inRange(dstMat2,Scalar(h1,s1,v1),Scalar(h2,s2,v2),dstMat2);
            findContours(dstMat2, contours_inRANGE, RETR_LIST, CHAIN_APPROX_SIMPLE);
            for(int k = 0; k < contours_inRANGE.size(); k++) {
                approxPolyDP(contours_inRANGE[k],contours_inRANGE[k],10,true);
                drawContours(dstMat2, contours_inRANGE,k,Scalar(0,0,255));
            }
            imshow("CzerwonyKontur",dstMat2);
            if(contours_inRANGE.size()>0) {
                Rect r = boundingRect(contours_inRANGE.at(0));
                int pom = 1;
                if(r.x < 125 && r.y<150) {
                    pom = 2;
                }
                if(r.x > 125 && r.y<150) {
                    pom = 1;
                }
                if(r.x > 125 && r.y>150) {
                    pom = 0;
                }
                if(r.x < 125 && r.y>150) {
                    pom = 3;
                }
                for(int k = 0; k < pom; k ++)
                    rotate(dstMat,dstMat,ROTATE_90_CLOCKWISE);

                imwrite("skan.jpg",dstMat);
            }
            imshow("oknoObrocone", dstMat);
        }

        imshow("frame_from_cam",frame);
    }
    return 0;
}