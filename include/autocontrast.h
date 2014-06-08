#ifndef _Autocontrast_h
#define _Autocontrast_h

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
using std::max;
using std::min;

#include "matrix.h"
#include "io.h"
class Autocontrast
{
public:
    int brightness[256];
    Image autocontr_filtr(Image);
    void count_thresholds(Image);
    Autocontrast(Image,double);
    ~Autocontrast();
    Autocontrast(const Autocontrast&) = delete;
    Autocontrast& operator=(const Autocontrast&) = delete;
private:
    int threshold1;
    int threshold2;
    int _rows;
    int _cols;
    double ** _y;
    double ** _new_y;
    int gap;
    
};

#endif
