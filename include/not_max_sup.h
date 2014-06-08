#ifndef _Not_Max_Sup_h
#define _Not_Max_Sup_h

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
#include "sobel.h"

class Not_max_suppression
{
private:
    int _r_num;
    int _c_num;
    double** _angles;
    double** _old_grad;
public:
    void fill_grad_border();
    Not_max_suppression(Sobel&);
    ~Not_max_suppression();
    Not_max_suppression(const Not_max_suppression&) = delete;
    Not_max_suppression& operator=(const Not_max_suppression&) = delete;
    int put_r() { return _r_num; }
    int put_c() { return _c_num; }
    void gradient_proc();
    double** grad_proc;
};


#endif
