#ifndef _Borders_h
#define _Borders_h


#include <sstream>
#include <iostream>
#include <fstream>
#include <initializer_list>
#include <limits>
#include <cmath>
#include <tuple>
#include <iterator>
#include <set>
#include <vector>
#include <queue>

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
class Borders
{
public:
    void hysteresis();
    bool norm_state(int,int);
    bool act_state(int,int);
    Borders(const Borders&) = delete;
    Borders& operator=(const Borders&) = delete;
    Borders(int,int,int,int,double**);
    ~Borders();
    vector<set<int>> areas;
    //set<int> eq_to_one;
    //vector<int> eq_to_1;
    int** border_map_mod;
    int** border_map;
    int low_bord;
    int high_bord;
    int put_rn(){ return _r_num; }
    int put_cn(){ return _c_num; }
    void find_areas();
    void find_eq_to_1(int);
    void find_eq_to_1_fast();
    void find_eq_to_1_fast_new(bool* eqs);
    void change_new_map();
    void change_new_map_fast_new(bool*);
    void change_new_map_fast();
private:
    int _r_num;
    int _c_num;
};



#endif
