#include "partition.h"

#define RADIUS 2

Partition::Partition(Image src, int** border_map,int n_r,int n_c):
red_chnl(nullptr),green_chnl(nullptr),blue_chnl(nullptr),
red_row_num(0),red_col_num(0),green_row_num(0),green_col_num(0),blue_row_num(0),blue_col_num(0),
_new_foto_r(0),_new_foto_c(0),_new_foto(nullptr),_cut_num(0),_hor_diap(0.025*(src.n_rows)), _vert_diap(0.1*(src.n_cols)),
_bm_r(src.n_rows),_bm_c(src.n_cols),
_old_foto_r(src.n_rows),_old_foto_c(src.n_cols),
_bord_map(nullptr),_old_foto(nullptr)
{
    _bord_map = new int*[src.n_rows];
    for(unsigned int i = 0; i < src.n_rows; ++i)
    {
        _bord_map[i] = new int[src.n_cols];
    }
    _old_foto = new int*[src.n_rows];
    for(unsigned int i = 0; i < src.n_rows; ++i)
    {
        _old_foto[i] = new int[src.n_cols];
    }
    for (unsigned int i=0; i< src.n_rows; ++i)
    {
        for (unsigned int j=0; j<src.n_cols; ++j)
        {
            _bord_map[i][j]=border_map[i][j];
            int r,g,b;
            tie(r,g,b)=src(i,j);
            _old_foto[i][j]=r;
        }
    }
}


Partition::~Partition()
{
    for(int i = 0; i < red_row_num; ++i) delete [] red_chnl[i];
    delete []red_chnl;
    for(int i = 0; i < green_row_num; ++i) delete [] green_chnl[i];
    delete []green_chnl;
    for(int i = 0; i < blue_row_num; ++i) delete [] blue_chnl[i];
    delete []blue_chnl;
    for(int i = 0; i < _bm_r; ++i) delete [] _bord_map[i];
    delete []_bord_map;
    for(int i = 0; i < _old_foto_r; ++i) delete [] _old_foto[i];
    delete []_old_foto;
    for(int i = 0; i < _new_foto_r; ++i) delete [] _new_foto[i];
    delete []_new_foto;
}

void Partition::_make_new_foto(int rows,int cols)
{
    _new_foto_r=rows;
    _new_foto_c=cols;
    _new_foto = new int*[_new_foto_r];
    for(int i = 0; i < _new_foto_r; ++i)
    {
        _new_foto[i] = new int[_new_foto_c];
    }
}

void Partition::_make_new_red(int rows,int cols)
{
    red_row_num=rows;
    red_col_num=cols;
    red_chnl = new int*[red_row_num];
    for(int i = 0; i < red_row_num; ++i)
    {
        red_chnl[i] = new int[red_col_num];
    }
}

void Partition::_make_new_green(int rows,int cols)
{
    green_row_num=rows;
    green_col_num=cols;
    green_chnl = new int*[green_row_num];
    for(int i = 0; i < green_row_num; ++i)
    {
        green_chnl[i] = new int[green_col_num];
    }
}

void Partition::_make_new_blue(int rows,int cols)
{
    blue_row_num=rows;
    blue_col_num=cols;
    blue_chnl = new int*[blue_row_num];
    for(int i = 0; i < blue_row_num; ++i)
    {
        blue_chnl[i] = new int[blue_col_num];
    }
}


int Partition::count_shift_hor(int first, int last, int flag)
{
    int imax1=0,max1=-1,imax2=0,max2=-1;
    for (int i=first; i< last; ++i)
    {
        int count=0;
        for (int j=0; j<_old_foto_c; ++j)
            if (_bord_map[i][j]==255) ++count;
        if (count>=max1)
        {
            max1=count;
            imax1=i;
        }
    }
    int gap=3;
    for (int i=first; i< imax1-gap; ++i)
    {
        int count=0;
        for (int j=0; j<_old_foto_c; ++j)
            if (_bord_map[i][j]==255) ++count;
        if (count>=max2)
        {
            max2=count;
            imax2=i;
        }
    }
    for (int i=imax1+gap+1; i< last; ++i)
    {
        int count=0;
        for (int j=0; j<_old_foto_c; ++j)
            if (_bord_map[i][j]==255) ++count;
        if (count>=max2)
        {
            max2=count;
            imax2=i;
        }
    }
    if (flag==1) return (imax1>imax2)?imax1:imax2;
    else return (imax1>imax2)?imax2:imax1;
}


int Partition::count_shift_vert(int first, int last, int flag)
{
    int jmax1=0,max1=-1,jmax2=0,max2=-1;
    for (int j=first; j< last; ++j)
    {
        int count=0;
        for (int i=0; i<_old_foto_r; ++i)
            if (_bord_map[i][j]==255) ++count;
        if (count>max1)
        {
            max1=count;
            jmax1=j;
        }
    }
    int gap=floor(0.1*(jmax1-first));
    for (int j=first; j< jmax1-gap; ++j)
    {
        int count=0;
        for (int i=0; i<_old_foto_r; ++i)
            if (_bord_map[i][j]==255) ++count;
        if (count>max2)
        {
            max2=count;
            jmax2=j;
        }
    }
    for (int j=jmax1+gap+1; j< last; ++j)
    {
        int count=0;
        for (int i=0; i<_old_foto_r; ++i)
            if (_bord_map[i][j]==255) ++count;
        if (count>max2)
        {
            max2=count;
            jmax2=j;
        }
    }
    if (flag==1) return (jmax1>jmax2)?jmax1:jmax2;
    else return (jmax1>jmax2)?jmax2:jmax1;
}

void Partition::cut_off()
{
    int first_row=count_shift_hor(0,_hor_diap,1)+1;
    int last_row=count_shift_hor(_bm_r-_hor_diap,_bm_r,2)-1;
    int first_col=count_shift_vert(0,_vert_diap,1)+1;
    int last_col=count_shift_vert(_bm_c-_vert_diap,_bm_c,2)-1;
    _make_new_foto(last_row-first_row,last_col-first_col);
    _cut_num=first_row;
    for (int i=0; i< _new_foto_r; ++i)
    {
        for (int j=0; j<_new_foto_c; ++j)
        {
            _new_foto[i][j]=_old_foto[i+first_row][j+first_col];
        }
    }
}

void Partition::divide_into_chnls()
{
    int one_third=floor(_bm_r/3);
    int bord_blue=count_shift_hor(one_third-_hor_diap,one_third+_hor_diap,2)-1-_cut_num;
    _make_new_blue(bord_blue,_new_foto_c);

    for (int i=0; i< bord_blue; ++i)
    {
        for (int j=0; j<_new_foto_c; ++j)
        {
            blue_chnl[i][j]=_new_foto[i][j];
        }
    }
    int bord_green_down=count_shift_hor(one_third-_hor_diap,one_third+_hor_diap,1)-_cut_num;
    int bord_green_up=count_shift_hor(2*one_third-_hor_diap,2*one_third+_hor_diap,2)-_cut_num;
    _make_new_green(bord_green_up-bord_green_down,_new_foto_c);
    for (int i=bord_green_down; i< bord_green_up; ++i)
    {
        for (int j=0; j<_new_foto_c; ++j)
        {
            green_chnl[i-bord_green_down][j]=_new_foto[i][j];
        }
    }
    int bord_red=count_shift_hor(2*one_third-_hor_diap,2*one_third+_hor_diap,1)+1-_cut_num;
    _make_new_red(_new_foto_r-bord_red,_new_foto_c);
    for (int i=bord_red; i< _new_foto_r; ++i)
    {
        for (int j=0; j<_new_foto_c; ++j)
        {
            red_chnl[i-bord_red][j]=_new_foto[i][j];
        }
    }
}

