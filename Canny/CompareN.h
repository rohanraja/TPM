class CompareN
{
    MatBoundary MB1, MB2, MB3 ;
    Mat src1, src2,src3, for_stitch1, for_stitch2;
   
    Mat main1, main2, main3 ;
public:
    
    double scalefactor = 0.3;
    bestMatchInfo bm;
    
    CompareN(vector<string> image_names)
    {
        
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

    Mat getResizedMat(char * ssrc)
    {
        Mat src = imread( ssrc, 1 );
        resize(src, src, Size(), scalefactor, scalefactor, INTER_CUBIC);
        return src;
    }


};