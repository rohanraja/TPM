class RepositionTwoIm
{
    Mat m1, m2, final1, final2;
public:
    RepositionTwoIm(Mat &mat1, Mat &mat2) : m1(mat1) , m2(mat2)
    {
    //    addImages();
     //   showImages();
        
        
    }
    
    Mat addImages(Point &p , Point &cnt_for_rot, float angleofrot)
    {
        final1 = Mat::zeros( 2000, 2000, m1.type() );
        final2 = Mat::zeros( 2000,2000, m2.type() );
        
        final1.setTo(Scalar(0,0,0));
        final2.setTo(Scalar(0,0,0));
        
        
        int pad = 50 ;
        
       

        copyMakeBorder(m1,final1,pad,pad,pad,pad,BORDER_CONSTANT,Scalar(0,0,0));
        
        copyMakeBorder(m2,final2,(final1.rows - m2.rows)/2,(final1.rows - m2.rows)/2,(final1.cols - m2.cols)/2,(final1.cols - m2.cols)/2,BORDER_CONSTANT,Scalar(0,0,0));
    
//        Mat out = Mat::zeros(final2.size(), final2.type());
//        final2(Rect(0,429, final2.cols,final2.rows-429)).copyTo(out(Rect(0,0,final2.cols,final2.rows-429)));
//        
//        final2 = out.clone() ;
//        
//        out = Mat::zeros(final1.size(), final1.type());
//        final1(Rect(109,0, final1.cols- 109,final1.rows)).copyTo(out(Rect(0,0,final1.cols-109,final1.rows)));
//        
//        final1 = out ;
        
      //  Point p(109,-429);
        
        final1 = translateImg(final1, p);
        
        Mat warp_mat = getRotationMatrix2D( Point(cnt_for_rot.x+pad, cnt_for_rot.y + pad ), angleofrot, 1 );
        
        warpAffine( final1, final1, warp_mat, final1.size() );

    //    resize(m1, final1, final1.size());
     //   resize(m2, final2, final2.size());
        Mat added = final1 + final2;
        
        return added;
        
    }
    
    void showImages()
    {
        namedWindow( "final1", CV_WINDOW_AUTOSIZE );
     
        imshow( "final1", final1 );
        
        namedWindow( "final2", CV_WINDOW_AUTOSIZE );
        
        imshow( "final2", final2 );
        
        namedWindow( "finalAdded", CV_WINDOW_AUTOSIZE );
        
        Mat added = final1 + final2;
        
        imshow( "finalAdded", added );
        
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