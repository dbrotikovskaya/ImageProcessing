#ifndef _Use_kernel_h
#define _Use_kernel_h

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
using std::min;
using std::max;

#include "matrix.h"
#include "io.h"

class Use_kernel
{
public:
    double** kernel;
    Use_kernel(Image src, double** mtx, int rows,int cols);
    ~Use_kernel();
    Image wide_im;
    Image new_im();
    void mirror();
    Use_kernel(const Use_kernel&) = delete;
    Use_kernel& operator=(const Use_kernel&) = delete;
private:
    int mtx_r;
    int mtx_c;
};

#endif
