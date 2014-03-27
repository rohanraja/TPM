class bestMatchInfo
{
    public :
    
    int sim_max , sim_idx, sim_order, rough_edge_id, best_mat_id, rough_edge_id_of_CANDI, candi_mat_id;
    double anglerot ;
    
    Point center_of_rot, CheckCaseStartPt ;
    
    bestMatchInfo(int sm, int si, int so, double aor , int edgeID = 0)
    {
        sim_max = sm;
        sim_idx = si;
        sim_order = so;
        anglerot = aor ;
        rough_edge_id = edgeID;
    }
    
    bestMatchInfo()
    {
        
    }
    
};

class TwoImgMatch
{
    newVector checkCase_nv,candidate_nv;
    
    MatBoundary candi_MB, checkCase_MB;
    Mat candi_m, checkCase_m;
    int candi_score ;
    double len_of_candi ;
     Mat for_stitch1, for_stitch2 ;
    vector<Point> conts ;
    double angle_of_rot ;
    
    Point bm_center ;

public:

    Point minC, maxC, diffC;
    vector<RoughEdge> checkCase_res;
    
     TwoImgMatch(MatBoundary &MB1, MatBoundary &MB2 ): candi_MB(MB1), checkCase_MB(MB2)
    {
    
        selectCandi();
        
        showMat(candi_m, "TEST FOR CANDIDATE");
        
        checkCase_res = checkCase_MB.rough_edges ;
        
        
    }
    
    bestMatchInfo getBestforallRES()
    {
        int sim, sim_max = INT_MAX, sim_idx, sim_order, simtmp ;
        
        bestMatchInfo bminfo ;
        
        for (int i=0; i < checkCase_res.size(); i++) {
            
            bestMatchInfo bm = checkSimwithRE(checkCase_res[i]);
            
            if (bm.sim_max < sim_max) {
                
                sim_max = bm.sim_max;
                
                bminfo = bm ;
                bminfo.rough_edge_id = i;
            }
        }
        
       
        int cnt_idx;
        
        if (bminfo.sim_order==1)
            cnt_idx = candidate_nv.num-1;
        else
            cnt_idx = 0;
        
        candidate_nv.translate_to_point(Point(15,15) - minC - bm_center);
        
        bminfo.center_of_rot = candidate_nv.pts[cnt_idx];
        
        bminfo.rough_edge_id_of_CANDI = candi_MB.roughest_id;
        
        conts = checkCase_MB.getEdgeSegment_NC(checkCase_res[bminfo.rough_edge_id]); //
        
        getVectorScoreforIDX(bminfo.sim_idx,bminfo.sim_order); // NOT NECESSARY
        
        return bminfo;
    }
    
    bestMatchInfo checkSimwithRE(RoughEdge checkCasere)
    {
        conts = checkCase_MB.getEdgeSegment_NC(checkCasere);
        
        bestMatchInfo bminfo;
        
        double t = (double)getTickCount();
        int sim, sim_max = INT_MAX, sim_idx, sim_order ;
        
        int icnt = 3 ;
        for (int i=0; i < conts.size(); i = i + icnt) {

            sim = getVectorScoreforIDX(i,1);
            
            if (sim < sim_max) {
                sim_max = sim;
                sim_idx = i;
                sim_order = 1;
                
                bminfo = *new bestMatchInfo(sim_max,sim_idx,sim_order,angle_of_rot);
                                
                bminfo.CheckCaseStartPt = conts[i] ;
                
            }

            sim = getVectorScoreforIDX(i,0);
            
            if (sim < sim_max) {
                sim_max = sim;
                sim_idx = i;
                sim_order = 0;
                bminfo = *new bestMatchInfo(sim_max,sim_idx,sim_order,angle_of_rot);
                
                bminfo.CheckCaseStartPt = conts[i] ;
                
            }
            
        }
        
        t = ((double)getTickCount() - t)/getTickFrequency();
        cout << "Times passed in seconds: " << t << endl;
        
        cout << "\n Minimum Similarity = " << 100 - sim_max << " %";
        cout << "\n Minimum Similarity INDEX = " << sim_idx;
        cout << "\n Minimum Similarity INDEX = " << sim_order;
        
        return bminfo;

    }
    
    int can_proceed ;
    
    int selectCandi()
    {
        RoughEdge roughest = candi_MB.getBestRE() ;
        
        
        
        if(roughest.roughness_scores == -1)
            can_proceed = 0;
        else
            can_proceed = 1;
        
        candidate_nv = candi_MB.getCandi_from_RE(roughest);
        
        bm_center = candidate_nv.pts[0];
        
        candidate_nv.translate_to_point(candidate_nv.pts[0]);
        
         maxC = candidate_nv.getMaxCoord();
         minC = candidate_nv.getMinCoord();
         diffC = maxC - minC ;
        
        candidate_nv.translate_to_point(minC - Point(15,15));
        
        candi_m = candidate_nv.plotPoints(1,diffC.x, diffC.y) ;
        
        len_of_candi = norm(candidate_nv.pts[0] - candidate_nv.pts[candidate_nv.num-1]) ;
        
        candi_score = calcIntfor(candi_m);
        
        return can_proceed;
        
    }
    
    Point center ;
    
    int getVectorScoreforIDX(int startIdx = 0, int iscnt = 1 )
    {
        
        int cnt_idx ;
        
        if (iscnt==1)
            cnt_idx = candidate_nv.num-1;
        else
            cnt_idx = 0;
        
        
        center = candidate_nv.pts[cnt_idx];
        Point endpt = candidate_nv.pts[candidate_nv.num-1 - cnt_idx];
        
        checkCase_nv = *new newVector();
        checkCase_nv = getPoint_AT_Dist(conts[startIdx],len_of_candi, conts) ;
        
        int opti_end_idx = getIndexofPoint2(checkCase_nv.pts[checkCase_nv.num - 2], conts) ;
        
        checkCase_nv.translate_to_point(checkCase_nv.pts[0] -1*center);
        
        checkCase_m = checkCase_nv.plotPoints(4, diffC.x, diffC.y);
        
        Point v11 = checkCase_nv.pts[checkCase_nv.num-1] - center ;
        Point v22 = endpt - center ;
        
        angle_of_rot = get_signed_angle(v11,v22);
        
        Mat warp_mat = getRotationMatrix2D( center, angle_of_rot, 1 );
        
        Mat rotated = Mat::zeros( 10, 10, CV_8UC3 ); ;
        
        warpAffine( checkCase_m, rotated, warp_mat, checkCase_m.size() );
        
        Mat diff = candi_m - rotated  ;
        
        int score = calcIntfor(diff);
        
        
        addWeighted(rotated, 1, candi_m, 1, 0, rotated);
//
        namedWindow( "Rotated", CV_WINDOW_AUTOSIZE );
        imshow( "Rotated", rotated );
        
        cout << "SCORE = " << (score*100)/candi_score ;
        
        return (score*100)/candi_score;
        
    }
    
    int getIndexofPointCMP(Point &p, vector<Point> &contourss)
    {
        int idx = 0;
        
        for (int i =0; i<contourss.size(); i++) {
            if (contourss[i] == p) {
                idx = i ;
            }
        }
        
        return idx;
    }
    
    newVector getPoint_AT_Dist(Point &sp, float dist, vector<Point> &contourss)
    {
        
        vector<Point>  vp ;
        
        float tmpdist = -1;
        int pnt = getIndexofPointCMP(sp, contourss);
        int i = pnt;
        
        while (i < contourss.size()-1 + pnt  && tmpdist < dist) {
            tmpdist = norm(contourss[i%(contourss.size()-1)] - contourss[pnt]);
            vp.push_back(contourss[i%(contourss.size()-1)]);
            i++;
            
        }
        
        if(norm(contourss[i%(contourss.size()-1)] - contourss[pnt]) > dist)
        {
            
            Point p = contourss[i%(contourss.size()-1)] + contourss[i%(contourss.size()-1) - 1] ;
            p = p * 0.5;
            
            vp.push_back(p);
        }
        else
        {
            vp.clear();
            vp.push_back(sp);
            vp.push_back(sp);
        }
        
        
        newVector tmp(vp.size(), vp, 0, "DistVec");
        
        return tmp;
        
    }


};