
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

CascadeClassifier banana_cascade;

vector<Rect> find_bananas(Mat frame) {

    vector<Rect> bananas;
    cvtColor(frame,frame,COLOR_BGR2GRAY);
    equalizeHist(frame, frame);
    banana_cascade.detectMultiScale(frame, bananas, 1.05, 12, 0 |CASCADE_FIND_BIGGEST_OBJECT , Size(100, 100));
    return bananas;
}

int main( void ) {
    VideoCapture capture;

    //-- 1. Load the cascade
    if( !banana_cascade.load(  "banana_classifier.xml"  ) ) {
        return -9;
    };
    capture.open( -1 );
    if ( ! capture.isOpened() ) {
        return -7;
    }
    int last_x=0;
    int last_y=0;
    int bananaCounter=0;
    Mat frame;
    while ( capture.read( frame ) )
    {
        if( frame.empty() )
            return -1;

        for(auto banana:find_bananas(frame)) {
            if( last_x - banana.x < 80 && last_x - banana.x > -80 && last_y - banana.y < 80 && last_y - banana.y > -80 ) {

            }else
            {
                bananaCounter++;
                cout<<"Nowy banan +1: "<<bananaCounter<<endl;
            }
            last_x=banana.x;
            last_y=banana.y;
            break;
        }

        imshow( "obraz", frame );
        if( (waitKey( 1 )&0x0ff) == 27 ) return 0;
    }
    return 0;
}