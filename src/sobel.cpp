#include "sobel.h"

Sobel::Sobel(int x, int y):
_ix(nullptr),_iy(nullptr),r_num(x),c_num(y),g(nullptr),teta(nullptr)
{
    _ix = new int*[x];
    for(int i = 0; i < x; ++i)
    {
        _ix[i] = new int[y];
    }
    _iy = new int*[x];
    for(int i = 0; i < x; ++i)
    {
        _iy[i] = new int[y];
    }
    g = new double*[x];
    for(int i = 0; i < x; ++i)
    {
        g[i] = new double[y];
    }
    teta = new double*[x];
    for(int i = 0; i < x; ++i)
    {
        teta[i] = new double[y];
    }
}

Sobel::~Sobel()
{
    for(unsigned int i = 0; i < sizeof(_ix); ++i) delete _ix[i];
    delete []_ix;
    for(unsigned int i = 0; i < sizeof(_iy); ++i) delete _iy[i];
    delete []_iy;
    for(unsigned int i = 0; i < sizeof(g); ++i) delete g[i];
    delete []g;
    for(unsigned int i = 0; i < sizeof(teta); ++i) delete teta[i];
    delete []teta;
}


void Sobel::fill_new_im(Image& wide_im,Image src)
{
    for ( unsigned int i=1; i<src.n_rows+1; ++i)
        for ( unsigned int j=1; j<src.n_cols+1; ++j)
            wide_im(i,j)=src(i-1,j-1);
    for ( int i=0, n=1; i>=0; --i, n+=2)
        for( unsigned int j=1; j<wide_im.n_cols-1 ; ++j)
            wide_im(i,j)=wide_im(i+n,j);
    for ( unsigned int i=wide_im.n_rows-1, n=1; i<wide_im.n_rows; ++i, n+=2)
        for ( unsigned int j= 1; j<wide_im.n_cols-1; ++j)
            wide_im(i,j)=wide_im(i-n,j);
    for ( int j=0, n=1; j>=0; --j,n+=2 )
        for( unsigned int i=0; i< wide_im.n_rows; ++i )
            wide_im(i,j)=wide_im(i,j+n);
    for ( unsigned int j=wide_im.n_cols-1, n=1; j<wide_im.n_cols; ++j,n+=2 )
        for( unsigned int i=0; i< wide_im.n_rows; ++i )
            wide_im(i,j)=wide_im(i,j-n);
}


void Sobel::_count_ix(Image chnl)
{
    int sum;
    Image wide_im(chnl.n_rows+2,chnl.n_cols+2);
    fill_new_im(wide_im,chnl);
    for (unsigned int i=1; i<wide_im.n_rows-1; ++i)
    {
        for (unsigned int j=1; j<wide_im.n_cols - 1; ++j)
        {
            sum=0;
            int x2=0;
            for (unsigned int x = i-1; x <= i+1; ++x)
            {
                int y2=0;
                for (unsigned int y=j-1; y <= j+1; ++y)
                {
                    int im_val = get<0>(wide_im(x,y));
                    sum+=im_val*_kx[x2][y2];
                    ++y2;
                    
                }
                ++x2;
            }
            _ix[i-1][j-1]=sum;
        }
    }
}


void Sobel::_count_iy(Image chnl)
{
    int sum;
    Image wide_im(chnl.n_rows+2,chnl.n_cols+2);
    fill_new_im(wide_im,chnl);
    for (unsigned int i=1; i<wide_im.n_rows-1; ++i)
    {
        for (unsigned int j=1; j<wide_im.n_cols - 1; ++j)
        {
            sum=0;
            int x2=0;
            for (unsigned int x = i-1; x <= i+1; ++x)
            {
                int y2=0;
                for (unsigned int y=j-1; y <= j+1; ++y)
                {
                    int im_val = get<0>(wide_im(x,y));
                    sum+=im_val*_ky[x2][y2];
                    ++y2;
                    
                }
                ++x2;
            }
            _iy[i-1][j-1]=sum;
        }
    }
}

void Sobel::count_gradient(Image chnl)
{
    _count_ix(chnl);
    _count_iy(chnl);
    for(int i=0; i<r_num; ++i)
        for(int j=0; j<c_num; ++j)
            g[i][j]=sqrt(_ix[i][j]*_ix[i][j]+_iy[i][j]*_iy[i][j]);
}

void Sobel::count_direction(Image chnl)
{
    for(int i=0; i<r_num; ++i)
        for(int j=0; j<c_num; ++j)
            teta[i][j]=atan2(_iy[i][j],_ix[i][j]);
}