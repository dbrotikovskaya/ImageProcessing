#ifndef _Overlap_h
#define _Overlap_h

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
class Overlap
{
public:
    int change_size;
    int green_i_shift;
    int green_j_shift;
    int blue_i_shift;
    int blue_j_shift;
    int h_r;
    int h_g;
    int h_b;
    int height_r;
    int height_g;
    int height_b;
    int width;
    int** red;
    int** green;
    int** blue;
    int min_i_sh;
    int min_j_sh;
    void get_shift(Image,Image,Image);
    Image shift_the_im(Image,Image,Image);
    Image shifted_image(Image,Image,Image);
    void fill_chanels(Image,Image,Image);
    void free_space();
    void standart_deviation(int**,int**, int,int,int,int);
    void cross_correlation(int**,int**,int,int,int,int);
    Image bilin_interpolation(Image, int, int);
    Overlap(Image r,Image g,Image b);
    ~Overlap();
    Overlap(const Overlap&) = delete;
    Overlap& operator=(const Overlap&) = delete;
private:
    void _make_new_red(int rows,int cols);
    void _make_new_green(int rows,int cols);
    void _make_new_blue(int rows,int cols);
    
};



#endif
