#include "use_kernel.h"


Use_kernel::Use_kernel(Image src, double** mtx, int rows,int cols):
kernel(nullptr), wide_im(src.n_rows+2*floor(rows/2), src.n_cols+2*floor(cols/2)),mtx_r(rows),mtx_c(cols)
{
    kernel = new double*[rows];
    for(int i = 0; i < rows; ++i)
    {
        kernel[i] = new double[cols];
    }
    for (int i=0; i<rows; ++i)
        for ( int j=0; j<cols; ++j)
            kernel[i][j]=mtx[i][j];
    
    for ( unsigned int i=floor(rows/2); i<src.n_rows+floor(rows/2); ++i)
        for ( unsigned int j=floor(cols/2); j<src.n_cols+floor(cols/2); ++j)
            wide_im(i,j)=src(i-floor(rows/2),j-floor(cols/2));
}


Use_kernel::~Use_kernel()
{
    for(int i = 0; i < mtx_r; ++i) delete kernel[i];
    delete []kernel;
}

void Use_kernel::mirror()
{
    int shift_r =floor(mtx_r/2);
    int shift_c =floor(mtx_c/2);
    for ( int i=shift_r-1, n=1; i>=0; --i, n+=2)
        for( unsigned int j= shift_c; j<wide_im.n_cols- shift_c ; ++j)
            wide_im(i,j)=wide_im(i+n,j);
    for ( unsigned int i=wide_im.n_rows-shift_r, n=1; i<wide_im.n_rows; ++i, n+=2)
        for ( unsigned int j= shift_c; j<wide_im.n_cols- shift_c; ++j)
            wide_im(i,j)=wide_im(i-n,j);
    for ( int j=shift_c-1, n=1; j>=0; --j,n+=2 )
        for( unsigned int i=0; i< wide_im.n_rows; ++i )
            wide_im(i,j)=wide_im(i,j+n);
    for ( unsigned int j=wide_im.n_cols-shift_c, n=1; j<wide_im.n_cols; ++j,n+=2 )
        for( unsigned int i=0; i< wide_im.n_rows; ++i )
            wide_im(i,j)=wide_im(i,j-n);
}


Image Use_kernel::new_im()
{
    Image n_im(wide_im.n_rows-2*floor(mtx_r/2),wide_im.n_cols-2*floor(mtx_c/2));
    
    double sum,sum2,sum3;
    for (unsigned int i=floor(mtx_r/2); i<wide_im.n_rows-floor(mtx_r/2); ++i)
    {
        for (unsigned int j=floor(mtx_c/2); j<wide_im.n_cols - floor(mtx_c/2); ++j)
        {
            sum=0;
            sum2=0;
            sum3=0;
            int x2=0;
            for (unsigned int x = i-floor(mtx_r/2); x <= i+floor(mtx_r/2); ++x)
            {
                int y2=0;
                for (unsigned int y=j-floor(mtx_c/2); y <= j+floor(mtx_c/2); ++y)
                {
                    int r,g,b;
                    tie(r,g,b)=wide_im(x,y);
                    sum+=r*kernel[x2][y2];
                    sum2+=g*kernel[x2][y2];
                    sum3+=b*kernel[x2][y2];
                    ++y2;
                    
                }
                ++x2;
            }
            sum=max(0,min(255,static_cast<int>(sum)));
            sum2=max(0,min(255,static_cast<int>(sum2)));
            sum3=max(0,min(255,static_cast<int>(sum3)));
            n_im(i-floor(mtx_r/2), j-floor(mtx_c/2))=make_tuple(sum,sum2,sum3);
        }
    }
    return n_im;
}
