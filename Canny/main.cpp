// Created by Rohan Raja


int thickness_for_vectorCMP = 4;

int length_of_candidate_points = 150;

int candi_step = 10;


#include "DrawFunc.h"
#include "NewCorner.h"
#include "NewVector.h"
#include "MatBoundary.h"

#include "RepositionTwoIm.h"
//#include "VectorTrans.h"
#include "TwoImgMatch.h"
#include "CompareN.h"


#define PI 3.14159265

using namespace cv;
using namespace std;

// Compile with G++ : $ g++ `pkg-config --cflags --libs opencv` main.cpp

int thresh = 65;

/// Function header
void thresh_callback(int, void* );

MatBoundary mb;
MatBoundary mb2;
Mat drawing2;

 // THE PARAMETERS START


 // THE PARAMETERS END

/** @function main */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include<string>
#include<string.h>
#include <libgen.h>
#include <dirent.h>
#include<iostream>
#include<stdio.h>
using namespace std;

int checkifdir(string path)
{
    
    char *myDir = new char[path.length() + 1];
    strcpy(myDir, path.c_str());
    
    struct stat myStat;
    if ((stat(myDir, &myStat) == 0) && (((myStat.st_mode) & S_IFMT) == S_IFDIR)) {
        return 1;
    }
    
    return 0;
}


// dir - complete system folder name
vector<char *> getFilenames(string inpimgdir){
    class dirent *ent;
    DIR *dir = opendir(inpimgdir.c_str());
    char *inpimgpath;
    string temp_str;
    
    vector<char *> listofImgs ;
    // Initial Set of Images
    while ((ent = readdir (dir)) != NULL){
        
        temp_str = inpimgdir + "/" + ent->d_name ;
        inpimgpath = new char[temp_str.length() + 1];
        strcpy(inpimgpath, temp_str.c_str());
        
        if(!checkifdir(temp_str)){
            listofImgs.push_back(inpimgpath);	
        }
        
    }
    return listofImgs;
}


int main( int argc, char** argv )
{
    
    
    
    
    Mat drawing;
    /// Load source image and convert it to gray
    Mat src = imread( "/Users/rohanraja/Documents/Rails_Projects/Opensoft/santosh_kumar/bin/other files/Image003.jpg" , 1 );
    
  //  char tmp1[200] = "/Users/rohanraja/Documents/Rails_Projects/Opensoft/santosh_kumar/bin/other files/Image001.jpg" ;
  //  char tmp2[200] = "/Users/rohanraja/Documents/Rails_Projects/Opensoft/santosh_kumar/bin/other files/Image002.jpg" ;
 //   char tmp3[200] = "/Users/rohanraja/Documents/Rails_Projects/Opensoft/santosh_kumar/bin/other files/Image003.jpg" ;
    
    char tmp1[200] = "/Users/rohanraja/Downloads/Scans/Test 5/1A.jpg" ;
    char tmp2[200] = "/Users/rohanraja/Downloads/Scans/Test 5/1B.jpg" ;
    char tmp3[200] = "/Users/rohanraja/Downloads/Scans/Test 5/1C.jpg" ;
    char tmp4[200] = "/Users/rohanraja/Downloads/Scans/Test 5/1D.jpg" ;
    
//    char tmp1[200] = "/Users/rohanraja/Downloads/resources/Image005.jpg";
  //  char tmp2[200] = "/Users/rohanraja/Downloads/resources/Image006.jpg";
    
  //  char tmp3[200] = "/Users/rohanraja/Documents/Rails_Projects/Opensoft/santosh_kumar/bin/other files/Image003.jpg" ;
    vector<char *> allImgs ;
  //
    allImgs.push_back(tmp1);
    allImgs.push_back(tmp2);
    allImgs.push_back(tmp3);
    allImgs.push_back(tmp4);
    
    
    vector<char *> out = getFilenames(argv[1]);
    
    CompareN all_cmp(out);
    
    
    
//    CompareTwo cmp("/Users/rohanraja/Documents/Rails_Projects/Opensoft/santosh_kumar/bin/other files/Image003.jpg", "/Users/rohanraja/Documents/Rails_Projects/Opensoft/santosh_kumar/bin/other files/Image001.jpg", "/Users/rohanraja/Documents/Rails_Projects/Opensoft/santosh_kumar/bin/other files/Image002.jpg");
    
  //  CompareTwo cmp("/Users/rohanraja/Downloads/resources/Image005.jpg", "/Users/rohanraja/Downloads/resources/Image006.jpg", "/Users/rohanraja/Documents/Rails_Projects/Opensoft/santosh_kumar/bin/other files/Image002.jpg");
    
  //  cmp.findMostSimilar();
    
//    CompareN cmpn("/Users/rohanraja/Documents/Rails_Projects/Opensoft/santosh_kumar/bin/other files/Image003.jpg", "/Users/rohanraja/Documents/Rails_Projects/Opensoft/santosh_kumar/bin/other files/Image001.jpg");
    
 //   CompareN cm2("/Users/rohanraja/Downloads/Scans/Test 5/1D.jpg", "/Users/rohanraja/Downloads/Scans/Test 5/1B.jpg");
    
   //  CompareN cmpn("/Users/rohanraja/Downloads/resources/Image005.jpg", "/Users/rohanraja/Downloads/resources/Image006.jpg");
    
    char* source_window = "Source";

    
 //     createTrackbar( "TKBAR", source_window, &thresh, mb.contours[mb.maxAreaIdx].size(), thresh_callback );
   // thresh_callback(0,0);
   

    waitKey(0);
    return(0);
}


/** @function thresh_callback */
void thresh_callback(int, void* )
{
    cout << "Thresh = " << thresh <<",";
    
    Mat tmpdraw = drawing2.clone();
    
    MyFilledCircle(tmpdraw,mb.contours[mb.maxAreaIdx][thresh]);
    
    char strr[80], strr2[80], strr3[80];
    
    Point p1 = mb.contours[mb.maxAreaIdx][thresh];
    Point p2 = mb.contours[mb.maxAreaIdx][thresh-2];
    Point p3 = mb.contours[mb.maxAreaIdx][thresh+2];
    
    string str = to_string(p1.x) + "," + to_string(p1.y) ;
    
    strcpy(strr, str.c_str());
    Point org(5,20);
    Point org2(5,40);
    Point org3(5,60);
   
    Point dp = p1 - p2;
    
    str = to_string(dp.x)+ "," + to_string(dp.y);
    strcpy(strr2, str.c_str());
    
    Point dp2 = p3 - p1;
    
    Point ddp = dp2 - dp;
    
    
    if (dp.x == 0) {
        str = "Grad= " + to_string((int)(90));
    }
    else
        str = "Grad= " + to_string((int)(atan (dp.y/dp.x) * 180 / PI));
    
    strcpy(strr3, str.c_str());
    
    tmpdraw = DisplayText(tmpdraw, strr,org );
    tmpdraw = DisplayText(tmpdraw, strr2,org2 );
    tmpdraw = DisplayText(tmpdraw, strr3,org3 );
    
   // VectTrans vtt(mb.contours[mb.maxAreaIdx], mb2, mb2.corners.size()/2 - thresh, 25, mb.cornerIndexes);
   //  VectTrans vtt(mb2.contours[mb.maxAreaIdx], mb, thresh, 30, mb2.cornerIndexes);
  //  vtt.findMostSimilar();
    
  //  VectTrans vvv(mb, mb2, thresh);
  //  vvv.solve();
//    imshow( "Contours", tmpdraw );
    
   // waitKey(1);
    
}