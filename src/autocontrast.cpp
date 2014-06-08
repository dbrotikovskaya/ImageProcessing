#include "autocontrast.h"

void Autocontrast::count_thresholds(Image src_image)
{
    //int num_of_pix=src_image.n_rows*src_image.n_cols;
    int sum=0;
    int i;
    
    for (i=0; sum<gap; ++i)
    {
        sum+=brightness[i];
        threshold1=i;
    }
    sum=0;
    for (i=255; sum<gap; --i)
    {
        sum+=brightness[i];
        threshold2=i;
    }

    if (gap==0)
    {
        i=0;
        while(brightness[i]==0)
        {
            i++;
            threshold1=i;
        }
        i=255;
        while(brightness[i]==0)
        {
            i--;
            threshold2=i;
        }
    }
}

Autocontrast::Autocontrast(Image src_image, double fraction):
threshold1(0),threshold2(255),_rows(src_image.n_rows),_cols(src_image.n_cols),_y(nullptr),_new_y(nullptr),gap(fraction*src_image.n_rows*src_image.n_cols)
{
    _y = new double*[src_image.n_rows];
    for(unsigned int i = 0; i < src_image.n_rows; ++i)
    {
        _y[i] = new double[src_image.n_cols];
    }
    _new_y = new double*[src_image.n_rows];
    for(unsigned int i = 0; i < src_image.n_rows; ++i)
    {
        _new_y[i] = new double[src_image.n_cols];
    }
    for(int i=0; i<256; ++i) brightness[i]=0;
}

Autocontrast::~Autocontrast()
{
    for(int i = 0; i < _rows; ++i) delete _y[i];
    delete []_y;
    for(int i = 0; i < _rows; ++i) delete _new_y[i];
    delete []_new_y;
}


Image Autocontrast::autocontr_filtr(Image src_image)
{
    for (unsigned int i=0; i<src_image.n_rows; ++i)
        for(unsigned int j=0; j<src_image.n_cols; ++j)
        {
            int r,g,b;
            tie(r,g,b)=src_image(i,j);
            _y[i][j]= 0.2125*r+0.7154*g+0.0721*b;
            brightness[static_cast<int>(_y[i][j])]+=1;
        }
    
    count_thresholds(src_image);
    if ((threshold2-threshold1)<=0) return src_image;
    
    for (unsigned int i=0; i<src_image.n_rows; ++i)
        for(unsigned int j=0; j<src_image.n_cols; ++j)
        {
            int r,g,b;
            int r1,g1,b1;
            tie(r,g,b)=src_image(i,j);
            
            if (r<threshold1) r1=0;
            else if (r>threshold2) r1=255;
            else r1=static_cast<double>((r-threshold1))/(threshold2-threshold1)*255.0;
            
            if (g<threshold1) g1=0;
            else if (g>threshold2) g1=255;
            else g1=static_cast<double>((g-threshold1))/(threshold2-threshold1)*255.0;
            
            if (b<threshold1) b1=0;
            else if (b>threshold2) b1=255;
            else b1=static_cast<double>((b-threshold1))/(threshold2-threshold1)*255.0;
            
            src_image(i,j)=make_tuple(static_cast<uint>(r1),static_cast<uint>(g1),static_cast<uint>(b1));
        }
    return src_image;
}

