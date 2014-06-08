#ifndef _Gauss_h
#define _Gauss_h

#include <sstream>
#include <iostream>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <cmath>
#include <tuple>

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

#include "matrix.h"
#include "io.h"
#define RADIUS 2
#define SIGMA 1.4

class Gauss_filter
{
public:
    Gauss_filter(int,double);
    ~Gauss_filter();
    void count_kernel_ord();
    void count_kernel_sep();
    void smooth_ord(Image& chnl);
    void smooth_sep(Image& chnl);
    void print_matr();
    Gauss_filter(const Gauss_filter&) = delete;
    Gauss_filter& operator=(const Gauss_filter&) = delete;
    void fill_new_kernel(Image& wide_im,Image src);
    void fill_vert(Image& wide_im,Image src);
    void fill_hor(Image& wide_im,Image src);
private:
    int _rad;
    double _sigma;
    int _size;
    double** _kernel_ord;
    double* _kernel_sep;
    
};



#endif
