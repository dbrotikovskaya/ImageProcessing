#include "overlap.h"


#define MAX_IM_SIZE 600
#define SIZE_CHANGE 2.0

Overlap::Overlap(Image r,Image g,Image b):
change_size(1),green_i_shift(0),green_j_shift(0),blue_i_shift(0),blue_j_shift(0),h_r(0),h_g(0),h_b(0),
height_r(r.n_rows),height_g(g.n_rows),height_b(b.n_rows),width(r.n_cols),red(nullptr),green(nullptr),blue(nullptr),
min_i_sh(0),min_j_sh(0)
{

}

int min2(int a,int b) { return (a<b)?a:b; }

int max2(int a,int b) { return (a>b)?a:b; }

Overlap::~Overlap()
{
    for(int i = 0; i < h_r; ++i) delete []red[i];
    delete []red;
    for(int i = 0; i < h_g; ++i) delete []green[i];
    delete []green;
    for(int i = 0; i < h_b; ++i) delete []blue[i];
    delete []blue;
}

int min3(int a,int b,int c)
{
    if (a<b)
        if (a<c) return a;
        else return c;
        else
            if (b<c) return b;
            else return c;
}


int max3(int a,int b,int c)
{
    if (a<b)
        if (b<c) return c;
        else return b;
        else
            if (a<c) return c;
            else return a;
}


void Overlap::free_space()
{
    for(int i = 0; i < h_r; ++i) delete []red[i];
    delete []red;
    for(int i = 0; i < h_g; ++i) delete []green[i];
    delete []green;
    for(int i = 0; i < h_b; ++i) delete []blue[i];
    delete []blue;
}



void Overlap::get_shift(Image r,Image g,Image b)
{
    if (max2(max3(r.n_rows,g.n_rows,b.n_rows),r.n_cols)>MAX_IM_SIZE)
    {
        get_shift(bilin_interpolation(r,r.n_rows/SIZE_CHANGE,r.n_cols/SIZE_CHANGE),
                  bilin_interpolation(g,g.n_rows/SIZE_CHANGE,g.n_cols/SIZE_CHANGE),
                  bilin_interpolation(b,b.n_rows/SIZE_CHANGE,b.n_cols/SIZE_CHANGE));
        green_i_shift*=SIZE_CHANGE;
        green_j_shift*=SIZE_CHANGE;
        blue_i_shift*=SIZE_CHANGE;
        blue_j_shift*=SIZE_CHANGE;
        fill_chanels(r,g,b);
        standart_deviation(red,green,h_r,h_g,green_i_shift,green_j_shift);
        green_i_shift=min_i_sh;
        green_j_shift=min_j_sh;
        standart_deviation(red,blue,h_r,h_b,blue_i_shift,blue_j_shift);
        blue_i_shift=min_i_sh;
        blue_j_shift=min_j_sh;
    }
    else
    {
        fill_chanels(r,g,b);
        standart_deviation(red,green,h_r,h_g,0,0);
        green_i_shift=min_i_sh;
        green_j_shift=min_j_sh;
        standart_deviation(red,blue,h_r,h_b,0,0);
        blue_i_shift=min_i_sh;
        blue_j_shift=min_j_sh;
    }
}

Image Overlap::shift_the_im(Image r,Image g,Image b)
{
    get_shift(r,g,b);
    return shifted_image(r,g,b);
}





Image Overlap::shifted_image(Image r,Image g,Image b)
{
    int rx1,rx2,ry1,ry2;
    int gx1,gy1;
    int bx1,by1;
    rx1=max3(green_i_shift,blue_i_shift,0);
    ry1=max3(green_j_shift,blue_j_shift,0);
    rx2=height_r - abs(min2(green_i_shift-(height_r-height_g),min2(blue_i_shift-(height_r-height_b),0)));
    ry2=width - abs(min3(green_j_shift,blue_j_shift,0));
    gx1=rx1-round(green_i_shift);
    gy1=ry1-round(green_j_shift);
    bx1=rx1-round(blue_i_shift);
    by1=ry1-round(blue_j_shift);
    Image fin_im(rx2-rx1,ry2-ry1);
    for (int i=0,ir=rx1,ig=gx1,ib=bx1; ir<rx2;++i, ++ir,++ig,++ib)
        for (int j=0,jr=ry1,jg=gy1,jb=by1; jr<ry2;++j, ++jr,++jg,++jb)
        {
            
            fin_im(i,j)=make_tuple(get<0>(r(ir,jr)),get<0>(g(ig,jg)),get<0>(b(ib,jb)));
        }
    return fin_im;
}

void Overlap::fill_chanels(Image r,Image g,Image b)
{
    _make_new_red(r.n_rows,r.n_cols);
    _make_new_green(g.n_rows,g.n_cols);
    _make_new_blue(b.n_rows,b.n_cols);
    h_r=r.n_rows;
    h_g=g.n_rows;
    h_b=b.n_rows;
    for (unsigned int i=0; i<r.n_rows; ++i)
        for (unsigned int j=0; j<r.n_cols; ++j)
            red[i][j]=get<0>(r(i,j));
    for (unsigned int i=0; i<g.n_rows; ++i)
        for (unsigned int j=0; j<g.n_cols; ++j)
            green[i][j]=get<0>(g(i,j));
    for (unsigned int i=0; i<b.n_rows; ++i)
        for (unsigned int j=0; j<b.n_cols; ++j)
            blue[i][j]=get<0>(b(i,j));
}



void Overlap::_make_new_red(int rows,int cols)
{
    red = new int*[rows];
    for(int i = 0; i < rows; ++i)
    {
        red[i] = new int[cols];
    }
}

void Overlap::_make_new_green(int rows,int cols)
{
    green = new int*[rows];
    for(int i = 0; i < rows; ++i)
    {
        green[i] = new int[cols];
    }
}

void Overlap::_make_new_blue(int rows,int cols)
{
    blue = new int*[rows];
    for(int i = 0; i < rows; ++i)
    {
        blue[i] = new int[cols];
    }
}



void Overlap::standart_deviation(int** chnl1,int** chnl2,int h1,int h2,int ish,int jsh)
{
    double mse, min_mse=1000;
    int flag=0;
    for (int i_shft=-15+ish; i_shft<=15+ish; ++i_shft)
        for(int j_shft=-15+jsh; j_shft<=15+jsh; ++j_shft)
        {
            mse=0;
            int i,i2,j,j2,max_i,max_j;
            i=max2(i_shft,0);
            max_i=min2(h1,h2+i_shft);
            i2=i-i_shft;
            for ( ; i<max_i; ++i, ++i2)
            {
                if (j_shft>=0)
                {
                    j2=0;
                    j=j_shft;
                    max_j=width;
                }
                else if(j_shft<0)
                {
                    j2=-j_shft;
                    j=0;
                    max_j=width+j_shft;
                    
                }
                for (; j<max_j; ++j, ++j2)
                {
                    mse+=static_cast<double>(pow((chnl1[i][j]-chnl2[i2][j2]),2)/((max_i-max2(i_shft,0))*(width-abs(j_shft))));
                }
            }
            if (flag==1)
            {
                if (mse<min_mse)
                {
                    min_mse=mse;
                    min_i_sh=i_shft;
                    min_j_sh=j_shft;
                }
            }
            else
            {
                flag=1;
                min_mse=mse;
                min_i_sh=i_shft;
                min_j_sh=j_shft;
            }
        }
}


void Overlap::cross_correlation(int** chnl1,int** chnl2,int h1,int h2,int ish,int jsh)
{
    double mse, max_mse=0;
    int flag=0;
    for (int i_shft=-15+ish; i_shft<=15+ish; ++i_shft)
        for(int j_shft=-15+jsh; j_shft<=15+jsh; ++j_shft)
        {
            mse=0;
            int i,i2,j,j2,max_i,max_j;
            i=max2(i_shft,0);
            max_i=min2(h1,h2+i_shft);
            i2=i-i_shft;
            
            for ( ; i<max_i; ++i, ++i2)
            {
                if (j_shft>=0)
                {
                    j2=0;
                    j=j_shft;
                    max_j=width;
                }
                else if(j_shft<0)
                {
                    j2=-j_shft;
                    j=0;
                    max_j=width+j_shft;
                    
                }
                for (; j<max_j; ++j, ++j2)
                {
                    mse+=static_cast<double>(chnl1[i][j]*chnl2[i2][j2]);
                }
            }
            if (flag==1)
            {
                if (mse>max_mse)
                {
                    max_mse=mse;
                    min_i_sh=i_shft;
                    min_j_sh=j_shft;
                }
            }
            else
            {
                flag=1;
                max_mse=mse;
                min_i_sh=i_shft;
                min_j_sh=j_shft;
            }
        }
}





Image Overlap::bilin_interpolation(Image old_image, int new_r_num, int new_c_num)
{
    int h, w;
    double t;
    double u;
    double tmp;
    double k1, k2, k3, k4;
    
    int r_1, g_1, b_1;
    int r_2, g_2, b_2;
    int r_3, g_3, b_3;
    int r_4, g_4, b_4;
    double r, g, b;
    
    int old_r_num = old_image.n_rows;
    int old_c_num = old_image.n_cols;
    
    Image new_image(new_r_num, new_c_num);
    
    for ( int i = 0; i < new_r_num; ++i )
    {
        tmp = static_cast<double>(i) / (new_r_num - 1) * (old_r_num - 1);
        h = floor(tmp);
        if (h < 0)
            h = 0;
        else
        {
            if (h >= old_r_num - 1)
                h = old_r_num - 2;
        }
        u = tmp - h;
        
        for ( int j = 0; j < new_c_num; ++j )
        {
            tmp = static_cast<double>(j) / (new_c_num - 1) * (old_c_num - 1);
            w = floor(tmp);
            if (w < 0)
                w = 0;
            else
            {
                if (w >= old_c_num - 1)
                    w = old_c_num - 2;
            }
            t = tmp - w;
            
            
            k1 = (1 - t) * (1 - u);
            k2 = (1 - t) * u;
            k3 = t * u;
            k4 = t * (1 - u);
            
            
            tie(r_1, g_1, b_1) = old_image(h,w);
            tie(r_2, g_2, b_2) = old_image(h + 1,w);
            tie(r_3, g_3, b_3) = old_image(h + 1,w + 1);
            tie(r_4, g_4, b_4) = old_image(h,w + 1);
            
            
            b = b_1 * k1 + b_2 * k2 + b_3 * k3 + b_4 * k4;
            g = g_1 * k1 + g_2 * k2 + g_3 * k3 + g_4 * k4;
            r = r_1 * k1 + r_2 * k2 + r_3 * k3 + r_4 * k4;
            
            
            new_image(i,j) = make_tuple(r, g, b);
        }
    }
    return new_image;
}

