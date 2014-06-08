#ifndef _Partition_h
#define _Partition_h

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

class Partition
{
public:
    int** red_chnl;
    int** green_chnl;
    int** blue_chnl;
    int red_row_num;
    int red_col_num;
    int green_row_num;
    int green_col_num;
    int blue_row_num;
    int blue_col_num;
    Partition(Image,int**,int,int);
    ~Partition();
    void cut_off();
    void divide_into_chnls();
    int count_shift_hor(int,int,int);
    int count_shift_vert(int,int,int);
    Partition(const Partition&) = delete;
    Partition& operator=(const Partition&) = delete;
    int _new_foto_r;
    int _new_foto_c;
    int** _new_foto;
    int put_r() { return _old_foto_r; }
    int put_c() { return _old_foto_c; }
    int put_old(int i,int j) { return _old_foto[i][j]; }
    
    
private:
    void _make_new_red(int,int);
    void _make_new_green(int,int);
    void _make_new_blue(int,int);
    void _make_new_foto(int,int);
    int _cut_num;
    int _hor_diap;
    int _vert_diap;
    int _bm_r;
    int _bm_c;
    int _old_foto_r;
    int _old_foto_c;
    int** _bord_map;
    int** _old_foto;
};

#endif
