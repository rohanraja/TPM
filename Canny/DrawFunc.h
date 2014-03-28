#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI 3.14159265

using namespace cv;
using namespace std;

void MyFilledCircle( Mat img, Point center, int bl = 0 )
{
    int thickness = -1;
    int lineType = 8;
    
    circle( img,
           center,5,
           Scalar( bl, 0, 155 ),
           thickness,
           lineType );
}


Mat DisplayText( Mat image, char * str, Point &org)
{
    Size textsize = getTextSize(str, CV_FONT_HERSHEY_COMPLEX,  0.4, 5, 0);
    
    
    int lineType = 2;
    
    Mat image2;
    
    //    for( int i = 0; i < 255; i += 2 )
    //    {
    //        image2 = image - Scalar::all(i);
    
    
    
    putText( image, str, org, CV_FONT_HERSHEY_COMPLEX, 0.6,
            Scalar(0,0, 255), 1, lineType );
    
    
    //  }
    
    return image;
}


int calcIntfor(Mat &image1)
{
    int basescore = 0;
    
    for (int i=0; i < image1.rows; i++) {
        for (int j=0; j < image1.cols; j++) {
            Vec3b intensity = image1.at<Vec3b>(i,j);
            basescore+= intensity[0] + intensity[1] + intensity[2];
        }
        
    }
    
    return basescore;
}


double getSlopeTan(Point &v11)
{
    double tanangle1 ;
    
    if(v11.x == 0)
    {
        if (v11.y > 0)
            tanangle1 = 90;
        else
            tanangle1 = -90;
    }
    else
    {
        tanangle1 = atan(float((float)v11.y /(float) v11.x));
        tanangle1 = (tanangle1 * 180) / PI ;
        if (v11.y < 0 && v11.x < 0) {
            tanangle1 = tanangle1 - 180 ;
        }
        if (v11.y > 0 && v11.x < 0) {
            tanangle1 = 180 + tanangle1;
        }
    }
    if(v11.y == 0)
    {
        if (v11.x > 0)
            tanangle1 = 0;
        else
            tanangle1 = 180;
    }
    
    
    return tanangle1;
}

double get_signed_angle(Point &v11, Point &v22)
{
    float ss = v11.x*v22.x + v11.y*v22.y;
    ss = ss / (norm(v11)*norm(v22)) ;
    ss = acos(ss) ;
    ss = (ss * 180) / PI ;
    
    int anglegrad = -1;
    double tanangle1, tanangle2;
    
    tanangle1 = getSlopeTan(v11);
    tanangle2 = getSlopeTan(v22);
    
    if(tanangle2>tanangle1)
        anglegrad = -1;
    else
        anglegrad = 1;
    
    return anglegrad*ss; ;
}

void showMat(Mat &m, char *windowname)
{
    namedWindow( windowname, CV_WINDOW_AUTOSIZE );
    imshow(windowname, m);
    
    char ext[200] ;
    strcpy(ext,windowname);
    
    strcat(ext, ".jpg");
    
    char basedir[200] = "./results/";
    
    strcat(basedir, ext);
    
    imwrite( basedir, m );
}