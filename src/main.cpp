#include <string>
#include <sstream>
#include <cstring>
#include <iostream>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <cmath>
#include <tuple>
#include <iterator>
#include <set>
#include <vector>


using namespace std;
using std::string;
using std::stringstream;
using std::cout;
using std::cerr;
using std::endl;
using std::numeric_limits;
using std::tuple;
using std::make_tuple;
using std::tie;

#include "io.h"
#include "matrix.h"
#include "gauss.h"
#include "sobel.h"
#include "not_max_sup.h"
#include "borders.h"
#include "partition.h"
#include "overlap.h"
#include "autocontrast.h"
#include "parser.h"
#include "use_kernel.h"


#define MIN_VAL 0.05
#define RADIUS 2
#define SIGMA 1.4
#define MAX_IM_SIZE 600
#define SIZE_CHANGE 2


void print_help(const char *argv0)
{
    const char *usage =
R"(where PARAMS are from list:

--align [--gray-world | --unsharp | --autocontrast [<fraction>]]
    align images with one of postprocessing functions

--gaussian <sigma> [<radius>=1]
    gaussian blur of image, 0.1 < sigma < 100, radius = 1, 2, ...

--gaussian-separable <sigma> [<radius>=1]
    same, but gaussian is separable

--sobel-x
    Sobel x derivative of image

--sobel-y
    Sobel y derivative of image

--unsharp
    sharpen image

--gray-world
    gray world color balancing

--autocontrast [<fraction>=0.0]
    autocontrast image. <fraction> of pixels must be croped for robustness

--resize <scale>
    resize image with factor scale. scale is real number > 0

--canny <threshold1> <threshold2>
    apply Canny filter to grayscale image. threshold1 < threshold2,
    both are in 0..360

--custom <kernel_string>
    convolve image with custom kernel, which is given by kernel_string, example:
    kernel_string = '1,2,3;4,5,6;7,8,9' defines kernel of size 3

[<param>=default_val] means that parameter is optional.
)";
    cout << "Usage: " << argv0 << " <input_image_path> <output_image_path> "
         << "PARAMS" << endl;
    cout << usage;
}

template<typename ValueType>
ValueType read_value(string s)
{
    stringstream ss(s);
    ValueType res;
    ss >> res;
    if (ss.fail() or not ss.eof())
        throw string("bad argument: ") + s;
    return res;
}

template<typename ValueT>
void check_number(string val_name, ValueT val, ValueT from,
                  ValueT to=numeric_limits<ValueT>::max())
{
    if (val < from)
        throw val_name + string(" is too small");
    if (val > to)
        throw val_name + string(" is too big");
}

void check_argc(int argc, int from, int to=numeric_limits<int>::max())
{
    if (argc < from)
        throw string("too few arguments for operation");

    if (argc > to)
        throw string("too many arguments for operation");
}

Matrix<int> parse_kernel(string kernel)
{
    // Kernel parsing implementation here
    return Matrix<int>(0, 0);
}



Image widen(int rows, int cols, int** chnl, int r_num, int c_num)
{
    Image im(rows,cols);
    int delta=r_num-im.n_rows;
    int first_bord=floor(delta/2)+delta%2;
    int sec_bord=floor(delta/2);
    for (int i=first_bord; i<r_num-sec_bord; ++i)
        for (int j=0; j<c_num; ++j)
            im(i-first_bord,j)=make_tuple(chnl[i][j],chnl[i][j],chnl[i][j]);
    return im;
}

int min(int a,int b) { return (a>b)?b:a; }
int max(int a,int b) { return (a<b)?b:a; }



Image gray_world(Image src_image)
{
    double s=0,s_r=0,s_g=0,s_b=0;
    
    for (unsigned int i=0; i<src_image.n_rows; ++i)
        for(unsigned int j=0; j<src_image.n_cols; ++j)
        {
            int r,g,b;
            tie(r,g,b)=src_image(i,j);
            s_r+=r;
            s_g+=g;
            s_b+=b;
        }
    s_r/=src_image.n_rows*src_image.n_cols;
    s_g/=src_image.n_rows*src_image.n_cols;
    s_b/=src_image.n_rows*src_image.n_cols;
    s=(s_r+s_g+s_b)/3;
    for (unsigned int i=0; i<src_image.n_rows; ++i)
        for(unsigned int j=0; j<src_image.n_cols; ++j)
        {
            int r,g,b;
            tie(r,g,b)=src_image(i,j);
            int n_r,n_g, n_b;
            if (s_r<MIN_VAL) n_r=min(255,r+s); else n_r=min(255,r*s/s_r);
            if (s_g<MIN_VAL) n_g=min(255,g+s); else n_g=min(255,g*s/s_g);
            if (s_b<MIN_VAL) n_b=min(255,b+s); else n_b=min(255,b*s/s_b);
            src_image(i,j)=make_tuple(min(255,n_r),min(255, n_g) ,min(255,n_b));
        }
    return src_image;
}




Image unsharp(Image src_image)
{
    double shrp[3][3]={
        {-1.0/6.0, -2.0/3.0, -1.0/6.0},
        {-2.0/3.0,  13.0/3.0, -2.0/3.0},
        {-1.0/6.0, -2.0/3.0, -1.0/6.0}
    };
    Image wid_im(src_image.n_rows+2,src_image.n_cols+2);
    for (unsigned int i=0; i<src_image.n_rows; ++i)
        for(unsigned int j=0; j<src_image.n_cols; ++j)
            wid_im(i+1,j+1)=src_image(i,j);
    for(unsigned int j=0; j<wid_im.n_cols; ++j)
    {
        wid_im(0,j)=wid_im(1,j);
        wid_im(wid_im.n_rows-1,j)=wid_im(wid_im.n_rows-2,j);
    }
    for(unsigned int i=1; i<wid_im.n_rows-1; ++i)
    {
        wid_im(i,0)=wid_im(i,1);
        wid_im(i,wid_im.n_cols-1)=wid_im(i,wid_im.n_cols-2);
    }
    
    double sum_r,sum_g,sum_b;
    for (unsigned int i=1; i<wid_im.n_rows - 1; ++i)
    {
        for (unsigned int j=1; j<wid_im.n_cols - 1; ++j)
        {
            sum_r=0;
            sum_g=0;
            sum_b=0;
            int x2=0;
            for (unsigned int x = i-1; x <= i+1; ++x)
            {
                int y2=0;
                for (unsigned int y=j-1; y <= j+1; ++y)
                {
                    int val_r = get<0>(wid_im(x,y));
                    int val_g = get<1>(wid_im(x,y));
                    int val_b = get<2>(wid_im(x,y));
                    sum_r+=val_r*shrp[x2][y2];
                    sum_g+=val_g*shrp[x2][y2];
                    sum_b+=val_b*shrp[x2][y2];
                    ++y2;
                    
                }
                ++x2;
            }
            int n_r=max(0,min(255,static_cast<int>(sum_r)));
            int n_g=max(0,min(255,static_cast<int>(sum_g)));
            int n_b=max(0,min(255,static_cast<int>(sum_b)));
            src_image(i-1,j-1)=make_tuple(n_r,n_g,n_b);
        }
    }
    return src_image;
}


Image resize(Image& old_image, int new_r_num, int new_c_num)
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

Image sobelx(Image src)
{
    const int _kx[3][3]= {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    int sumr;
    int sumg;
    int sumb;
    Image wide_im(src.n_rows+2,src.n_cols+2);
    Image res(src.n_rows,src.n_cols);
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
    for (unsigned int i=1; i<wide_im.n_rows-1; ++i)
    {
        for (unsigned int j=1; j<wide_im.n_cols - 1; ++j)
        {
            sumr=0;
            sumg=0;
            sumb=0;
            int x2=0;
            for (unsigned int x = i-1; x <= i+1; ++x)
            {
                int y2=0;
                for (unsigned int y=j-1; y <= j+1; ++y)
                {
                    int r,g,b;
                    tie(r,g,b)=wide_im(x,y);
                    sumr+=r*_kx[x2][y2];
                    sumg+=g*_kx[x2][y2];
                    sumb+=b*_kx[x2][y2];
                    ++y2;
                }
                ++x2;
            }
            sumr=max(0,min(255,sumr));
            sumg=max(0,min(255,sumg));
            sumb=max(0,min(255,sumb));
            res(i-1,j-1)=make_tuple(sumr,sumg,sumb);
        }
    }
    return res;
}

Image sobely(Image src)
{
    const int _ky[3][3]= {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}
    };
    int sumr;
    int sumg;
    int sumb;
    Image wide_im(src.n_rows+2,src.n_cols+2);
    Image res(src.n_rows,src.n_cols);
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
    for (unsigned int i=1; i<wide_im.n_rows-1; ++i)
    {
        for (unsigned int j=1; j<wide_im.n_cols - 1; ++j)
        {
            sumr=0;
            sumg=0;
            sumb=0;
            int x2=0;
            for (unsigned int x = i-1; x <= i+1; ++x)
            {
                int y2=0;
                for (unsigned int y=j-1; y <= j+1; ++y)
                {
                    int r,g,b;
                    tie(r,g,b)=wide_im(x,y);
                    sumr+=r*_ky[x2][y2];
                    sumg+=g*_ky[x2][y2];
                    sumb+=b*_ky[x2][y2];
                    ++y2;
                    
                }
                ++x2;
            }
            sumr=max(0,min(255,sumr));
            sumg=max(0,min(255,sumg));
            sumb=max(0,min(255,sumb));
            res(i-1,j-1)=make_tuple(sumr,sumg,sumb);
        }
    }
    return res;
}


Image sobel_x(Image src_image)
{
    Image filt_image = src_image;
    Sobel sob_filter(filt_image.n_rows,filt_image.n_cols);
    sob_filter.count_gradient(filt_image);
    Image dst_image(sob_filter.r_num,sob_filter.c_num);
    for(unsigned int i=0; i<dst_image.n_rows;++i)
        for(unsigned int j=0;j<dst_image.n_cols; ++j)
        {
            int val;
            val=static_cast<int>(max(0,min(255,sob_filter.put_ix(i,j))));
            dst_image(i,j)=make_tuple(val,val,val);
        }
    return dst_image;
}

Image sobel_y(Image src_image)
{
    Image filt_image = src_image;
    Sobel sob_filter(filt_image.n_rows,filt_image.n_cols);
    sob_filter.count_gradient(filt_image);
    Image dst_image(sob_filter.r_num,sob_filter.c_num);
    for(unsigned int i=0; i<dst_image.n_rows;++i)
        for(unsigned int j=0;j<dst_image.n_cols; ++j)
        {
            int val;
            val=static_cast<int>(max(0,min(255,sob_filter.put_iy(i,j))));
            dst_image(i,j)=make_tuple(val,val,val);
        }
    return dst_image;
}

Image canny(Image src_image, int thr1, int thr2)
{
    Image filt_image = src_image;
    
    Gauss_filter g_filt(RADIUS,SIGMA);
    g_filt.smooth_sep(filt_image);
    
    Sobel sob_filter(filt_image.n_rows,filt_image.n_cols);
    sob_filter.count_gradient(filt_image);
    sob_filter.count_direction(filt_image);
    Not_max_suppression n_m_filt(sob_filter);
    n_m_filt.gradient_proc();
    Borders b_map(thr1,thr2,sob_filter.r_num,sob_filter.c_num,n_m_filt.grad_proc);
    b_map.hysteresis();
    Image dst_image(b_map.put_rn(),b_map.put_cn());
    for(unsigned int i=0; i<dst_image.n_rows;++i)
        for(unsigned int j=0;j<dst_image.n_cols; ++j)
        {
            int val;
            val=static_cast<int>(max(0,min(255,b_map.border_map_mod[i][j])));
            dst_image(i,j)=make_tuple(val,val,val);
        }
    return dst_image;
}


Image gaussian(Image src_image,double  sigma, int radius)
{
    Image mid_image = src_image;
    Gauss_filter g_filt(radius,sigma);
    g_filt.smooth_ord(mid_image);
    
    return mid_image;
}


Image gaussian_separable(Image src_image,double  sigma, int radius)
{
    Image mid_image = src_image;
    Gauss_filter g_filt(radius,sigma);
    g_filt.smooth_sep(mid_image);
    
    return mid_image;
}


Image align(Image src_image,char* name)
{
    Image filt_image =load_image(name);
    
    Gauss_filter g_filt(RADIUS,SIGMA);
    g_filt.smooth_sep(filt_image);
    
    Sobel sob_filter(filt_image.n_rows,filt_image.n_cols);
    sob_filter.count_gradient(filt_image);
    sob_filter.count_direction(filt_image);
    
    Not_max_suppression n_m_filt(sob_filter);
    n_m_filt.gradient_proc();
    Borders b_map(70,130,sob_filter.r_num,sob_filter.c_num,n_m_filt.grad_proc);
    b_map.hysteresis();
    Partition crop_foto(src_image,b_map.border_map_mod,b_map.put_rn(),b_map.put_cn());
    crop_foto.cut_off();
    crop_foto.divide_into_chnls();
    /*int min_r=min(crop_foto.red_row_num,min(crop_foto.green_row_num,crop_foto.blue_row_num));
    int min_c=min(crop_foto.red_col_num,min(crop_foto.green_col_num,crop_foto.blue_col_num));
    Image red=widen(min_r,min_c,crop_foto.red_chnl,crop_foto.red_row_num,crop_foto.red_col_num);
    Image green=widen(min_r,min_c,crop_foto.green_chnl,crop_foto.green_row_num,crop_foto.green_col_num);
    Image blue=widen(min_r,min_c,crop_foto.blue_chnl,crop_foto.blue_row_num,crop_foto.blue_col_num);*/
    
    
     Image red(crop_foto.red_row_num,crop_foto.red_col_num);
     Image green(crop_foto.green_row_num,crop_foto.green_col_num);
     Image blue(crop_foto.blue_row_num,crop_foto.blue_col_num);
     
     for(unsigned int i=0; i<red.n_rows;++i)
     for(unsigned int j=0;j<red.n_cols; ++j)
     red(i,j)=make_tuple(crop_foto.red_chnl[i][j],crop_foto.red_chnl[i][j],crop_foto.red_chnl[i][j]);
     
     for(unsigned int i=0; i<green.n_rows;++i)
     for(unsigned int j=0;j<green.n_cols; ++j)
     green(i,j)=make_tuple(crop_foto.green_chnl[i][j],crop_foto.green_chnl[i][j],crop_foto.green_chnl[i][j]);
     
     for(unsigned int i=0; i<blue.n_rows;++i)
     for(unsigned int j=0;j<blue.n_cols; ++j)
     blue(i,j)=make_tuple(crop_foto.blue_chnl[i][j],crop_foto.blue_chnl[i][j],crop_foto.blue_chnl[i][j]);
     
    Overlap ov_the_im(red,green,blue);
    Image dst_image=ov_the_im.shift_the_im(red,green,blue);
    return dst_image;
}


Image align2(Image src_image, string postpr,char* name)
{
    if (postpr=="--gray-world")
    {
        Image dst_image=gray_world(align(src_image,name));
        return dst_image;
    }
        Image dst_image=unsharp(align(src_image,name));
        return dst_image;
}

Image align3(Image src_image, double fraction,char* name)
{
    Image mid_res=align(src_image,name);
    Autocontrast a_filt(mid_res,fraction);
    Image dst_image=a_filt.autocontr_filtr(mid_res);
    return dst_image;
}

int main(int argc, char **argv)
{
    try {
        check_argc(argc, 2);
        if (string(argv[1]) == "--help") {
            print_help(argv[0]);
            return 0;
        }
        check_argc(argc, 4);
        Image src_image = load_image(argv[1]);
        
        
        /*Image im(200,200);
        for ( int i = 0; i < 200; ++i )
            for ( int j = 0; j < 200; ++j )
                im(i,j) = make_tuple(255,0,0);
        save_image(im, "red.bmp");
        return 0;*/
        
        string action(argv[3]);

        if (action == "--sobel-x") {
            check_argc(argc, 4, 4);
            Image dst_image = sobelx(src_image);
            save_image(dst_image, argv[2]);
        } else if (action == "--sobel-y") {
            check_argc(argc, 4, 4);
            Image dst_image = sobely(src_image);
            save_image(dst_image, argv[2]);
        } else if (action == "--unsharp") {
            check_argc(argc, 4, 4);
            Image dst_image = unsharp(src_image);
            save_image(dst_image, argv[2]);
        } else if (action == "--gray-world") {
            check_argc(argc, 4, 4);
            Image dst_image = gray_world(src_image);
            save_image(dst_image, argv[2]);
        } else if (action == "--resize") {
            check_argc(argc, 5, 5);
            double scale = read_value<double>(argv[4]);
            Image dst_image = resize(src_image, src_image.n_rows*scale, src_image.n_cols*scale);
            save_image(dst_image, argv[2]);
        }  else if (action == "--custom") {
            check_argc(argc, 5, 5);
            // Matrix<double> kernel = parse_kernel(argv[4]);
            // Function custom is useful for making concrete linear filtrations
            // like gaussian or sobel. So, we assume that you implement custom
            // and then implement concrete filtrations using this function.
            // For example, sobel_x may look like this:
            // sobel_x (...) {
            //    Matrix<double> kernel = {{-1, 0, 1},
            //                             {-2, 0, 2},
            //                             {-1, 0, 1}};
            //    return custom(src_image, kernel);
            // }
            // dst_image = custom(src_image, kernel);
            Parser parse;
            parse.get_numbers(argv[4]);
            parse.fill_matrix();
            Use_kernel u_krnl(src_image,parse.kernel,parse.put_r(),parse.put_c());
            u_krnl.mirror();
            Image dst_image=u_krnl.new_im();
            save_image(dst_image, argv[2]);
            
        } else if (action == "--autocontrast") {
            check_argc(argc, 4, 5);
            double fraction = 0.0;
            if (argc == 5) {
                fraction = read_value<double>(argv[4]);
                check_number("fraction", fraction, 0.0, 0.4);
            }
            Autocontrast filt(src_image, fraction);
            Image dst_image = filt.autocontr_filtr(src_image);
            save_image(dst_image, argv[2]);
        } else if (action == "--gaussian" || action == "--gaussian-separable") {
            check_argc(argc, 5, 6);
            double sigma = read_value<double>(argv[4]);
            check_number("sigma", sigma, 0.1, 100.0);
            int radius = 3 * sigma;
            if (argc == 6) {
                radius = read_value<int>(argv[5]);
                check_number("radius", radius, 1);
            }
            if (action == "--gaussian") {
                Image dst_image = gaussian(src_image, sigma, radius);
                save_image(dst_image, argv[2]);
            } else {
                Image dst_image = gaussian_separable(src_image, sigma, radius);
                save_image(dst_image, argv[2]);
            }
        } else if (action == "--canny") {
            check_argc(6, 6);
            int threshold1 = read_value<int>(argv[4]);
            check_number("threshold1", threshold1, 0, 360);
            int threshold2 = read_value<int>(argv[5]);
            check_number("threshold2", threshold2, 0, 360);
            if (threshold1 >= threshold2)
                throw string("threshold1 must be less than threshold2");
            Image dst_image = canny(src_image, threshold1, threshold2);
            save_image(dst_image, argv[2]);
        } else if (action == "--align") {
            check_argc(argc, 4, 6);
            if (argc == 5|| argc == 6) {
                string postprocessing(argv[4]);
                if (postprocessing == "--gray-world" ||
                    postprocessing == "--unsharp") {
                    check_argc(argc, 5, 5);
                    Image dst_image = align2(src_image, postprocessing,argv[1]);
                    save_image(dst_image, argv[2]);
                } else if (postprocessing == "--autocontrast") {
                    double fraction = 0.0;
                    if (argc == 6) {
                        fraction = read_value<double>(argv[5]);
                        check_number("fraction", fraction, 0.0, 0.4);
                    }
                    Image dst_image = align3(src_image, fraction, argv[1]);
                    save_image(dst_image, argv[2]);
                } else {
                    throw string("unknown align option ") + postprocessing;
                }
            } else {
                Image dst_image = align(src_image,argv[1]);
                save_image(dst_image, argv[2]);
            }
        } else {
            throw string("unknown action ") + action;
        }
        
    } catch (const string &s) {
        cerr << "Error: " << s << endl;
        cerr << "For help type: " << endl << argv[0] << " --help" << endl;
        return 1;
    }
    return 0;
}
