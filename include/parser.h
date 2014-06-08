#ifndef _Parser_h
#define _Parser_h

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
#include "matrix.h"
#include "io.h"

class Parser
{
public:
    double** kernel;
    Parser():  kernel(nullptr),_num(), _n_rows(0), _n_cols(0) {};
    ~Parser();
    int put_r() { return _n_rows; }
    int put_c() { return _n_cols; }
    void fill_matrix();
    string str_without_bl(string s);
    void get_numbers(char* str);
    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
private:
    void _make_space();
    vector<double> _num;
    int _n_rows;
    int _n_cols;
};


#endif
