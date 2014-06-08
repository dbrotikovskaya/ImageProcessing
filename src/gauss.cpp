#include "gauss.h"

Gauss_filter::~Gauss_filter()
{
    for(int i = 0; i < _rad; ++i) delete _kernel_ord[i];
    delete []_kernel_ord;
    delete []_kernel_sep;
}

Gauss_filter::Gauss_filter(int radius, double sigma):
_rad(radius), _sigma(sigma), _size(2*radius+1), _kernel_ord(nullptr), _kernel_sep(nullptr)
{
    int size=2*radius+1;
    _kernel_ord = new double*[size];
    for(int i = 0; i < size; ++i)
    {
        _kernel_ord[i] = new double[size];
        for (int j=0; j < size; ++j) _kernel_ord[i][j]=0;
    }
    _kernel_sep = new double[size];
}

void Gauss_filter::count_kernel_ord()
{
    int origin = floor(_size / 2);
    double sum=0;
    for (int x = 0; x < _size; ++x)
    {
        for (int y=0; y < _size; ++y)
        {
            _kernel_ord[x][y]=exp(-((x-origin)*(x-origin)+(y-origin)*(y-origin))/(2*_sigma*_sigma));
            sum+=_kernel_ord[x][y];
        }
    }
    
    for (int x = 0; x < _size; ++x)
    {
        for (int y=0; y < _size; ++y)
        {
            _kernel_ord[x][y]= _kernel_ord[x][y]/sum;
        }
    }
    
}

void Gauss_filter::count_kernel_sep()
{
    int origin = floor(_size / 2);
    float sum=0;
    for (int x = 0; x < _size; ++x)
    {
        _kernel_sep[x]=exp(-(x-origin)*(x-origin)/(2*_sigma*_sigma));
        sum+=_kernel_sep[x];
    }
    for (int x = 0; x < _size; ++x)
    {
        _kernel_sep[x]=_kernel_sep[x]/sum;
    }
    
}

void Gauss_filter::fill_new_kernel(Image& wide_im,Image src)
{
    for ( unsigned int i=_rad; i<src.n_rows+_rad; ++i)
        for ( unsigned int j=_rad; j<src.n_cols+_rad; ++j)
            wide_im(i,j)=src(i-_rad,j-_rad);
    for ( int i=_rad-1, n=1; i>=0; --i, n+=2)
        for( unsigned int j= _rad; j<wide_im.n_cols-_rad ; ++j)
            wide_im(i,j)=wide_im(i+n,j);
    for ( unsigned int i=wide_im.n_rows-_rad, n=1; i<wide_im.n_rows; ++i, n+=2)
        for ( unsigned int j= _rad; j<wide_im.n_cols- _rad; ++j)
            wide_im(i,j)=wide_im(i-n,j);
    for ( int j=_rad-1, n=1; j>=0; --j,n+=2 )
        for( unsigned int i=0; i< wide_im.n_rows; ++i )
            wide_im(i,j)=wide_im(i,j+n);
    for ( unsigned int j=wide_im.n_cols-_rad, n=1; j<wide_im.n_cols; ++j,n+=2 )
        for( unsigned int i=0; i< wide_im.n_rows; ++i )
            wide_im(i,j)=wide_im(i,j-n);
}


void Gauss_filter::smooth_ord(Image& chnl)
{
    double sum,sum2,sum3;
    int origin = floor(_size / 2);
    count_kernel_ord();
    Image sm_chnl(chnl.n_rows+2*origin, chnl.n_cols+2*origin);
    fill_new_kernel(sm_chnl,chnl);
    for (unsigned int i=origin; i<sm_chnl.n_rows-origin; ++i)
    {
        for (unsigned int j=origin; j<sm_chnl.n_cols - origin; ++j)
        {
            sum=0;
            sum2=0;
            sum3=0;
            int x2=0;
            for (unsigned int x = i-origin; x <= i+origin; ++x)
            {
                int y2=0;
                for (unsigned int y=j-origin; y <= j+origin; ++y)
                {
                    int r,g,b;
                    tie(r,g,b)=sm_chnl(x,y);
                    sum+=r*_kernel_ord[x2][y2];
                    sum2+=g*_kernel_ord[x2][y2];
                    sum3+=b*_kernel_ord[x2][y2];
                    ++y2;
                    
                }
                ++x2;
            }
            chnl(i-origin, j-origin)=make_tuple(sum,sum2,sum3);
        }
    }
}

void Gauss_filter::fill_hor(Image& wide_im,Image src)
{
    for (unsigned int j=_rad; j<wide_im.n_cols-_rad; ++j)
        for( unsigned int i=0; i< wide_im.n_rows; ++i )
            wide_im(i,j)=src(i,j-_rad);
    for ( int j=_rad-1, n=1; j>=0; --j,n+=2 )
        for( unsigned int i=0; i< wide_im.n_rows; ++i )
            wide_im(i,j)=wide_im(i,j+n);
    for ( unsigned int j=wide_im.n_cols-_rad, n=1; j<wide_im.n_cols; ++j,n+=2 )
        for( unsigned int i=0; i< wide_im.n_rows; ++i )
            wide_im(i,j)=wide_im(i,j-n);
}

void Gauss_filter::fill_vert(Image& wide_im,Image src)
{
    for ( int i=_rad-1, n=1; i>=0; --i, n+=2)
        for( unsigned int j=0; j<wide_im.n_cols; ++j)
            wide_im(i,j)=wide_im(i+n,j);
    for ( unsigned int i=wide_im.n_rows-_rad, n=1; i<wide_im.n_rows; ++i, n+=2)
        for ( unsigned int j=0; j<wide_im.n_cols; ++j)
            wide_im(i,j)=wide_im(i-n,j);
}

void Gauss_filter::smooth_sep(Image& chnl)
{
    double sum,sum2,sum3;
    int origin = floor(_size / 2);
    //count_kernel_ord();
    count_kernel_sep();
    Image sm_chnl(chnl.n_rows, chnl.n_cols+2*origin);
    Image sm_chnl2(chnl.n_rows+2*origin, chnl.n_cols);
    fill_hor(sm_chnl,chnl);
    for (unsigned int i=0; i<sm_chnl.n_rows; ++i)
    {
        for (unsigned int j=origin; j<sm_chnl.n_cols - origin; ++j)
        {
            sum=0;
            sum2=0;
            sum3=0;
            int y2=0;
            for (unsigned int y=j-origin; y <= j+origin; ++y)
            {
                int r,g,b;
                tie(r,g,b)=sm_chnl(i,y);
                sum+=r*_kernel_sep[y2];
                sum2+=g*_kernel_sep[y2];
                sum3+=b*_kernel_sep[y2];
                ++y2;
                
            }
            sm_chnl2(i+origin, j-origin)=make_tuple(sum,sum2,sum3);
        }
    }
    fill_vert(sm_chnl2,chnl);
    for (unsigned int i=origin; i<sm_chnl2.n_rows-origin; ++i)
    {
        for (unsigned int j=0; j<sm_chnl2.n_cols; ++j)
        {
            sum=0;
            sum2=0;
            sum3=0;
            int x2=0;
            for (unsigned int x=i-origin; x <= i+origin; ++x)
            {
                int r,g,b;
                tie(r,g,b)=sm_chnl2(x,j);
                sum+=r*_kernel_sep[x2];
                sum2+=g*_kernel_sep[x2];
                sum3+=b*_kernel_sep[x2];
                ++x2;
                
            }
            chnl(i-origin, j)=make_tuple(sum,sum2,sum3);
        }
    }
}

void Gauss_filter::print_matr()
{
    for (int i=0; i<_size; ++i)
    {
        for (int j=0; j<_size; ++j)
            cout<<" "<<_kernel_ord[i][j]<<" ";
        cout<<"\n";
    }
    cout<<endl<<endl;
}

