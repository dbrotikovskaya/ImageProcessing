#ifndef _Sobel_h
#define _Sobel_h

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

class Sobel
{
private:
    const int _kx[3][3]= {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };
    const int _ky[3][3]= {
        {1, 2, 1},
        {0, 0, 0},
        {-1, -2, -1}
    };
    int** _ix;
    int** _iy;
    void _count_ix(Image);
    void _count_iy(Image);
public:
    int r_num;
    int c_num;
    int put_ix(int i,int j){ return _ix[i][j]; }
    int put_iy(int i,int j){ return _iy[i][j]; }
    double** g;
    double** teta;
    void count_gradient(Image);
    void count_direction(Image);
    void fill_new_im(Image& wide_im,Image chnl);
    Sobel(const Sobel&) = delete;
    Sobel& operator=(const Sobel&) = delete;
    Sobel(int,int);
    ~Sobel();
};

#endif
