int getIndexofPoint2(Point &p, vector<Point> &contourss)
{
    int idx = 0;
    
    for (int i =0; i<contourss.size(); i++) {
        if (contourss[i] == p) {
            idx = i ;
        }
    }
    
    return idx;
}

newVector getPoint_Dist2(Point &sp, float dist, vector<Point> &contourss)
{
    
    vector<Point>  vp ;
    
    float tmpdist = -1;
    int pnt = getIndexofPoint2(sp, contourss);
    int i = pnt;
    
    while (i < contourss.size()-1   && tmpdist < dist) {
        tmpdist = norm(contourss[i] - contourss[pnt]);
        vp.push_back(contourss[i]);
        i++;
        
    }
    int sze;
    
    if(norm(contourss[i] - contourss[pnt]) > dist)
    {
        sze = i - pnt + 1;
        
        Point p = contourss[i] + contourss[i-1] ;
        p = p * 0.5;
    
        vp.push_back(p);
    }
    else
    {
        sze = 2;
        vp.push_back(sp);
        vp.push_back(sp);
    }
    
    
    newVector tmp(sze, vp, 0, "DistVec");
    
    return tmp;
    
}

class VectTrans
{
    newVector a,b;
    Mat m1,m2, warp_mat;
    vector<Point> conts ;
    vector<int> vecIdx ;
    MatBoundary MB2 ;
public:
    newVector nv ;
    float p2dist;
    Point minC, maxC, diffC;
    VectTrans(MatBoundary mb2 ): MB2(mb2)
    {
        //   solve();
    }
    
    VectTrans(newVector v1, newVector v2) : a(v1), b(v2)
    {
        // v1.printAll();
        m1 = v1.plotPoints();
        
        // v2.printAll();
        m2 = v2.plotPoints();
        
        solve();
        
    }
    
    VectTrans(MatBoundary &mB1 , MatBoundary &mB2, int psize)
    {
        a = *new newVector(30,mB1.contours[mB1.maxAreaIdx],psize, "plot1", mB1);
        b = *new newVector(35,mB2.corners,mB2.corners.size()/2 -15, "plot2", mB2);
        m1 = a.plotPoints() ;
        m2 = b.plotPoints() ;
        
        //   solve();
    }
    
    
    int m2score = 0;
    int opti_end_idx, opti_start_idx ;
    float angle_of_rot ;
    int solve2(int startIdx = 0, int iscnt = 1 )
    {
        
        int cnt_idx ;
        
        if (iscnt==1)
            cnt_idx = b.num-1;
        else
            cnt_idx = 0;
        
        
        Point center = b.pts[cnt_idx];
        Point endpt = b.pts[b.num-1 - cnt_idx];
        
        nv = *new newVector();
        nv = getPoint_Dist2(conts[startIdx],p2dist, conts) ;
        opti_end_idx = getIndexofPoint2(nv.pts[nv.num - 2], conts) ;
        
        nv.translate_to_point(nv.pts[0] -1*center);
        
        Mat nvmat = nv.plotPoints(2, diffC.x, diffC.y);
        
        Point v11 = nv.pts[nv.num-1] - center ;
        Point v22 = endpt - center ;
        
        float ss = v11.x*v22.x + v11.y*v22.y;
        ss = ss / (norm(v11)*norm(v22)) ;
        ss = acos(ss) ;
        ss = (ss * 180) / PI ;
        
        int anglegrad = -1;
        float tanangle1, tanangle2;
        
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
        
        if(v22.x == 0)
        {
            if (v22.y > 0)
                tanangle2 = 90;
            else
                tanangle2 = -90;
        }
        else
        {    tanangle2 = atan(float((float)v22.y /(float) v22.x));
             tanangle2 = (tanangle2 * 180) / PI ;
            if (v22.y < 0 && v22.x < 0) {
                tanangle2 =  tanangle2 -180 ;
            }
            if (v22.y > 0 && v22.x < 0) {
                tanangle2 = 180 + tanangle2;
            }
            
        }
        if(v22.y == 0)
        {
            if (v22.x > 0)
                tanangle2 = 0;
            else
                tanangle2 = 180;
        }
        
        if(tanangle2>tanangle1)
            anglegrad = -1;
        else
            anglegrad = 1;
        
        warp_mat = getRotationMatrix2D( center, anglegrad*ss, 1 );
        
        angle_of_rot = anglegrad*ss;
        
        Mat rotated = Mat::zeros( 10, 10, CV_8UC3 ); ;
        
        
        warpAffine( nvmat, rotated, warp_mat, nvmat.size() );
        
        
      //  double t = (double)getTickCount();
        
        Mat diff = m2 -rotated  ;
        
        int score = 0;
        for (int i=0; i < diff.rows; i++) {
            for (int j=0; j < diff.cols; j++) {
                Vec3b intensity = diff.at<Vec3b>(i,j);
                score+= intensity[0] + intensity[1] + intensity[2];
            }
            
        }
        
//        t = ((double)getTickCount() - t)/getTickFrequency();
//        cout << "Times passed for Solve in seconds: " << t << endl;
//        String strr = to_string(((score*100)/m2score)) + " %";
//        char stt[80];
//        strcpy(stt, strr.c_str()) ;
//        Point org(50,50);
//        diff = DisplayText(diff,stt ,org );
//        
        addWeighted(rotated, 1, m2, 1, 0, rotated);
//        
//        namedWindow( "Diff", CV_WINDOW_AUTOSIZE );
//        imshow( "Diff", diff );
//        
        namedWindow( "Rotated", CV_WINDOW_AUTOSIZE );
        imshow( "Rotated", rotated );
        
        
        return (score*100)/m2score;
        
    }
    
    int solve(int iscnt = 1)
    {
        
        
        
        float p2dist = norm(b.pts[0] - b.pts[b.num-1]) ;
        int cnt_idx ;
        
        if (iscnt==1)
            cnt_idx = b.num-1;
        else
            cnt_idx = 0;
        
        Point center = b.pts[cnt_idx];
        Point endpt = b.pts[b.num-1 - cnt_idx];
        
        nv = *new newVector(a);
        nv = a.getPoint_Dist(a.pts[0],p2dist) ;
        
        nv.translate_to_point(nv.pts[0] -1*center);
        
        Mat nvmat = nv.plotPoints(4);
        
        Point v11 = nv.pts[nv.num-1] - center ;
        Point v22 = endpt - center ;
        
        float ss = v11.x*v22.x + v11.y*v22.y;
        ss = ss / (norm(v11)*norm(v22)) ;
        ss = acos(ss) ;
        ss = (ss * 180) / PI ;
        
        warp_mat = getRotationMatrix2D( center, ss, 1 );
        
        Mat rotated = Mat::zeros( 1000, 1000, CV_8UC3 ); ;
        
        warpAffine( nvmat, rotated, warp_mat, nvmat.size() );
        
        Mat diff = m2 -rotated  ;
        
        int score = 0;
        for (int i=0; i < diff.rows; i++) {
            for (int j=0; j < diff.cols; j++) {
                Vec3b intensity = diff.at<Vec3b>(i,j);
                score+= intensity[0] + intensity[1] + intensity[2];
            }
            
        }
        
        String strr = to_string(score);
        char stt[80];
        strcpy(stt, strr.c_str()) ;
        Point org(50,50);
        diff = DisplayText(diff,stt ,org );
        
        addWeighted(rotated, 1, m2, 1, 0, rotated);
        
        namedWindow( "Diff", CV_WINDOW_AUTOSIZE );
        imshow( "Diff", diff );
        
        namedWindow( "Rotated", CV_WINDOW_AUTOSIZE );
        imshow( "Rotated", rotated );
        
        return score;
    }
    
    int sim_order = 1 ;
    
    int findMostSimilar(int st = 0, int len = 10, vector<Point> &countt = *new vector<Point> )
    {
        conts = countt;
        
        b = *new newVector(len,MB2.corners,st, "Compare Case", MB2);
        b.translate_to_point(b.pts[0] );
        
        maxC = b.getMaxCoord();
        minC = b.getMinCoord();
        diffC = maxC - minC ;
        
        b.translate_to_point(minC - Point(15,15));
        
        
        m2 = b.plotPoints(1,diffC.x, diffC.y) ;
        
        p2dist = norm(b.pts[0] - b.pts[b.num-1]) ;
        
        double t = (double)getTickCount();
        int sim, sim_max = INT_MAX, sim_idx ;
        
        int score = 0;
        for (int i=0; i < m2.rows; i++) {
            for (int j=0; j < m2.cols; j++) {
                Vec3b intensity = m2.at<Vec3b>(i,j);
                score+= intensity[0] + intensity[1] + intensity[2];
            }
            
        }
        
        m2score = score;
        
        int icnt = 10 ;
        for (int i=0; i < conts.size(); i = i + icnt) {
            
            
            /*
             sim = solve2(vecIdx[i],1);
            
            if (sim < sim_max) {
                sim_max = sim;
                sim_idx = vecIdx[i];
                sim_order = 1;
                
                if(sim_max < 70)
                {
                    for (int j= vecIdx[i-3]; j < vecIdx[i+3]; j++) {
                        sim = solve2(j,1);
                        
                        
                        if (sim < sim_max) {
                            sim_max = sim;
                            sim_idx = j;
                            sim_order = 1;
                            
                    }
                        
                    }
                }
                
                //   cout << "\n***Min Found" << sim_idx ;
                
            }
           // waitKey(1);
            
            sim = solve2(i,0);
            
            if (sim < sim_max) {
                sim_max = sim;
                sim_idx = i;
                sim_order = 0;
                
                if(sim_max < 70)
                {
                    for (int j= vecIdx[i-1]; j < vecIdx[i+1]; j++) {
                        sim = solve2(j,0);
                        
                        if (sim < sim_max) {
                            sim_max = sim;
                            sim_idx = j;
                            sim_order = 0;
                        }
                        
                    }
                }
                //   cout << "\n***Min Found" << sim_idx ;
              //  waitKey(1);
            }
            
            */
            
            sim = solve2(i,1);
            
            if (sim < sim_max) {
                sim_max = sim;
                sim_idx = i;
                sim_order = 1;
               
                
            }
            
            

            
     //      waitKey(1);
//
          //  cout << sim_max << "\n\n";
//            if(sim<65)
//            {
//                if(icnt > 1)
//                {
//                    i = i - 10;
//                    
//                     icnt = 1;
//                //    cout << "\n**********ICNT CHANGED TO : " << icnt << "\n" ;
//                }
//               
//                
//            }
//            
//            if(sim<80)
//            {
//                if(icnt > 10)
//                {
//                    i = i - 10;
//                    
//                    icnt = 10;
//                //    cout << "\n**********ICNT CHANGED TO : " << icnt << "\n" ;
//                }
//                
//                
//            }
//            
//            if(sim < 95)
//            {
//                if(icnt > 15)
//                {
//                    i-=5;
//                    icnt = 15;
//               //     cout << "\n**********ICNT CHANGED TO : " << icnt << "\n"  ;
//                    
//                }
//                
//            }
//            
//            if(sim > 90)
//            {
//                if(icnt < 10)
//                {
//                    icnt = 10;
//                //    cout << "\n**********ICNT CHANGED TO : " << icnt << "\n"  ;
//                    
//                }
//                
//            }
          //  waitKey(1);
            
            sim = solve2(i,0);
            
            if (sim < sim_max) {
                sim_max = sim;
                sim_idx = i;
                sim_order = 0;
                
            }
//
            
//
            
//            if(sim<65)
//            {
//                if(icnt > 1)
//                {
//                    i = i - 10;
//                    
//                    icnt = 1;
//                    //    cout << "\n**********ICNT CHANGED TO : " << icnt << "\n" ;
//                }
//                
//                
//            }
//            
//            if(sim<80)
//            {
//                if(icnt > 10)
//                {
//                    i = i - 10;
//                    
//                    icnt = 10;
//                    //    cout << "\n**********ICNT CHANGED TO : " << icnt << "\n" ;
//                }
//                
//                
//            }
//            
//            if(sim < 95)
//            {
//                if(icnt > 15)
//                {
//                    i-=5;
//                    icnt = 15;
//                    //     cout << "\n**********ICNT CHANGED TO : " << icnt << "\n"  ;
//                    
//                }
//                
//            }
//            
//            if(sim > 90)
//            {
//                if(icnt < 10)
//                {
//                    icnt = 10;
//                    //    cout << "\n**********ICNT CHANGED TO : " << icnt << "\n"  ;
//                    
//                }
//                
//            }
            
            //   cout << "\n\nSIM = " << sim;
            
            
        }
        
        t = ((double)getTickCount() - t)/getTickFrequency();
        cout << "Times passed in seconds: " << t << endl;
        
        cout << "\n Minimum Similarity = " << 100 - sim_max << " %";
        cout << "\n Minimum Similarity INDEX = " << sim_idx;
        cout << "\n Minimum Similarity INDEX = " << sim_order;
        
        
        
        solve2(sim_idx, sim_order);
        opti_start_idx = sim_idx;
        return sim_max;
        
        
    }
    
    
    
};


class CompareTwo
{
    MatBoundary MB1, MB2, MB3 ;
    Mat src1, src2,src3;
    Mat main1, main2, main3 ;
public:
    
    CompareTwo(char * m1src, char * m2src,  char * m3src)
    {
        src1 = imread( m1src, 1 );
        resize(src1, src1, Size(), 0.3, 0.3, INTER_CUBIC);
        main1 = src1.clone();
        MB1 = *new MatBoundary(src1) ;
        src1 = MB1.getBoundary();
        Mat test = MB1.getCorners(3);
        
        namedWindow( "TEST CORNERS", CV_WINDOW_AUTOSIZE );
        imshow("TEST CORNERS", test);
        
        
        src2 = imread( m2src, 1 );
        resize(src2, src2, Size(), 0.3, 0.3, INTER_CUBIC);
        main2 = src2.clone();
        MB2 = *new MatBoundary(src2) ;
        src2 = MB2.getBoundary();
        MB2.getCorners(3);
        
        src3 = imread( m3src, 1 );
        resize(src3, src3, Size(), 0.3, 0.3, INTER_CUBIC);
        main3 = src3.clone();
        MB3 = *new MatBoundary(src3) ;
        MB3.getBoundary();
        MB3.getCorners(3);
        
                
    }
    
    void findMostSimilar(int stidx = 15, int numvecc = 15)
    {
        // Take one sure part from P1, test for best match with all other parts
        
        VectTrans vtt(MB2);
        
        char* source_window = "Compare Main";
        
        namedWindow( source_window, CV_WINDOW_AUTOSIZE );
        
        MyFilledCircle(src2, MB2.corners[stidx] );
         MyFilledCircle(src2, MB2.corners[stidx+numvecc] );
        imshow( source_window, src2 );
        moveWindow(source_window, 600, 0);
        
        vtt.findMostSimilar(stidx,numvecc,MB1.contours[MB1.maxAreaIdx]);
        
        namedWindow( "IM1", CV_WINDOW_AUTOSIZE );
        
        cout << MB2.corners[stidx+numvecc] << "\n" << MB1.contours[MB1.maxAreaIdx][vtt.opti_start_idx];
        
        MyFilledCircle(src1, MB1.contours[MB1.maxAreaIdx][vtt.opti_start_idx] );
        MyFilledCircle(src1, MB1.contours[MB1.maxAreaIdx][vtt.opti_end_idx] );
      //  MyFilledCircle(src1, MB2.corners[stidx+numvecc] );
        imshow( "IM1", src1 );
        
        Point p, cnt_for_rot;
        
        if(vtt.sim_order ==1)
            cnt_for_rot = MB2.corners[stidx+numvecc] ;
        else
            cnt_for_rot = MB2.corners[stidx];
        
        p = MB1.contours[MB1.maxAreaIdx][vtt.opti_start_idx] - cnt_for_rot ;
        
        RepositionTwoIm rp(src1, src2);
        rp.addImages(p,cnt_for_rot , vtt.angle_of_rot);
        rp.showImages();
                
//        for(int i = 255; 330 - 30; i++)
//        {
//            cout << "\ni = " << i;
//            
//            vtt.findMostSimilar(i,15,MB1);
//            waitKey(100);
//        }
    }
};