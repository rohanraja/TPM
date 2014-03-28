class matchTrack
{
public:
    
    int mat_id, rough_edge_id ;
    
    matchTrack(int mid, int rid)
    {
        mat_id = mid;
        rough_edge_id = rid;
    }
};

struct RotationData
{
    double angle;
    Point center_of_rot ;
};

class TotalChange
{
    public :
    
    double angle;
    vector<RotationData> rotation_data ;
    vector<Point> translations ;
    
    TotalChange()
    {
    }
    
    void addChanges(bestMatchInfo &bm)
    {
        RotationData rd ;
        rd.angle = bm.anglerot ;
        rd.center_of_rot = bm.center_of_rot ;
        rotation_data.push_back(rd);
        translations.push_back(bm.getTransVector());
    }
    
};

class CompareN
{
    MatBoundary MB1, MB2, MB3 ;
    Mat src1, src2,src3, for_stitch1, for_stitch2;
   
    Mat main1, main2, main3 ;
public:
    
    double scalefactor = 0.3;
    bestMatchInfo bm;
    
    vector<Mat> src_mats ;
    vector<Mat> for_stitches ;
    MatBoundary MatBs[40] ;
    
    vector<TotalChange> mat_changes ;
    
    vector<matchTrack> match_Tracks;
    
    vector<bestMatchInfo> allMatches ;
    
    CompareN(vector<char *> image_names)
    {
        Mat tmp_src, for_stitch_tmp;
        MatBoundary tmpB;
        
        mat_changes = *new vector<TotalChange>(image_names.size()) ;
        
        for(int i=0 ; i<image_names.size(); i++)
        {
            tmp_src = getResizedMat(image_names[i]) ;
            tmpB = *new MatBoundary(tmp_src) ;
            
            for_stitch_tmp = tmpB.getBoundary();
            tmpB.getRoughedges();
            
            showMat(for_stitch_tmp, "FORSHOW");
            src_mats.push_back(tmp_src);
            MatBs[i] = tmpB ;
            for_stitches.push_back(for_stitch_tmp);
            
        }
        
        solve();
      //  stitch_mats(allMatches[0]);
        
        stitch_all();
        
        
    }
    
    void solve()
    {
        int cur_candi = 0;
        
        while(cur_candi < src_mats.size() - 1)
        {
            bestMatchInfo bminfo ;
            int sim, sim_max = INT_MAX, sim_idx, sim_order, simtmp ;
            
            for(int i=0 ; i<src_mats.size(); i++)
            {
               if(i!=cur_candi)
               {
                   TwoImgMatch twoM(MatBs[cur_candi], MatBs[i]);
                   
                   if(twoM.can_proceed == 0)
                       continue;
                   
                   bestMatchInfo bmtmp = twoM.getBestforallRES();
                   
                   if (bmtmp.sim_max < sim_max) {
                       
                       sim_max = bmtmp.sim_max;
                       bminfo = bmtmp ;
                       bminfo.best_mat_id = i;
                       bminfo.candi_mat_id = cur_candi;
                    
                   }
                   
               }
                
            }
            
            MatBs[bminfo.best_mat_id].rough_edges[bminfo.rough_edge_id].is_used = 1;
            
            MatBs[bminfo.candi_mat_id].rough_edges[bminfo.rough_edge_id_of_CANDI].is_used = 1;
            
            

//            
            for(int k=0; k<allMatches.size();k++)
            {
                if(allMatches[k].best_mat_id == bminfo.best_mat_id)
                    mat_changes[allMatches[k].candi_mat_id].addChanges(bminfo);
            
                if(allMatches[k].candi_mat_id == bminfo.best_mat_id)
                    mat_changes[allMatches[k].best_mat_id].addChanges(bminfo);
            
            }
            
            allMatches.push_back(bminfo);
            
            mat_changes[bminfo.best_mat_id].addChanges(bminfo);
            
            cur_candi++ ;
        
        }
        
    }
    
    int pad = 200 ;
    
    Mat get_final_rotated(int the_id)
    {
        Mat final = Mat::zeros( 2000, 2000, for_stitches[the_id].type() );
        final.setTo(Scalar(0,0,0));
        
        copyMakeBorder(for_stitches[the_id],final,pad,pad,pad,pad,BORDER_CONSTANT,Scalar(0,0,0));
        
        
        
        for(int k=0; k < mat_changes[the_id].rotation_data.size() ; k++)
        {
            
            
           
            final = translateImg(final, mat_changes[the_id].translations[k]);
            
            Point cnt_for_rot = mat_changes[the_id].rotation_data[k].center_of_rot ;
            double angleofrot = mat_changes[the_id].rotation_data[k].angle ;
            
            Mat warp_mat = getRotationMatrix2D( Point(cnt_for_rot.x+pad, cnt_for_rot.y + pad ), angleofrot, 1 );
            
            warpAffine( final, final, warp_mat, final.size() );
                        
            
        }
        
        return final;
        
    }
    
    
    void stitch_all()
    {
        Mat mega_FINAL =  get_final_rotated(0) ;
        
        for(int i=1 ; i<mat_changes.size(); i++)
        {
            Mat tmpfinal = get_final_rotated(i);
            
            mega_FINAL = mega_FINAL + tmpfinal;
            
        }
        
        showMat(mega_FINAL, "SHOWDOWN" );
    }
    
    void stitch_mats(bestMatchInfo bminfo )
    {
        Point center_for_rot = bminfo.center_of_rot;
        Point trans_vector = bminfo.getTransVector();
        
        Mat fs1 = for_stitches[bminfo.candi_mat_id];
        Mat fs2 = for_stitches[bminfo.best_mat_id];
        
        RepositionTwoIm rp(fs2, fs1);
        
        Mat frst = rp.addImages(trans_vector,center_for_rot , bminfo.anglerot);
        
        rp.showImages();
        
    }
    
    CompareN(char * m1src, char * m2src)
    {
        src1 = getResizedMat(m1src);
        src2 = getResizedMat(m2src);
        
        MB1 = *new MatBoundary(src1) ;
        for_stitch1 = MB1.getBoundary();
        MB1.getRoughedges();
        
        MB2 = *new MatBoundary(src2) ;
        for_stitch2 = MB2.getBoundary();
        MB2.getRoughedges();

      
        TwoImgMatch twoM(MB1, MB2);
        
        bm = twoM.getBestforallRES();
        
        cout << "The score is : " << bm.sim_max ;
       
      //  MyFilledCircle(for_stitch1, bm.center_of_rot);
        
      //  showMat(for_stitch1, "CHECK START");
        
        stitch();

    }
    
    void stitch()
    {
        Point center_for_rot = bm.center_of_rot;
        Point trans_vector = bm.CheckCaseStartPt - center_for_rot;
        
        
        RepositionTwoIm rp(for_stitch2, for_stitch1);
        
        Mat frst = rp.addImages(trans_vector,center_for_rot , bm.anglerot);
        rp.showImages();

        
    }

    Mat getResizedMat(string ssrc)
    {
        Mat src = imread( ssrc, 1 );
        resize(src, src, Size(), scalefactor, scalefactor, INTER_CUBIC);
        return src;
    }
    Mat getResizedMat(char * ssrc)
    {
        Mat src = imread( ssrc, 1 );
        resize(src, src, Size(), scalefactor, scalefactor, INTER_CUBIC);
        return src;
    }
    
    Mat translateImg(Mat &m, Point &p)
    {
        Mat map_x(m.size(), CV_32FC1);
        Mat map_y(m.size(), CV_32FC1);
        Mat dst(m.size(), m.type());
        
        for( int j = 0; j < m.rows; j++ )
        {
            for( int i = 0; i < m.cols; i++ )
            {
                map_x.at<float>(j,i) = i + p.x;
                map_y.at<float>(j,i) = j + p.y;
            }
        }
        
        remap( m, dst, map_x, map_y, CV_INTER_LINEAR, BORDER_CONSTANT, Scalar(0,0, 0) );
        
        return dst;
        
        
    }


};