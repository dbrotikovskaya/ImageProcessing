#include "not_max_sup.h"

Not_max_suppression::Not_max_suppression(Sobel& filt):
_r_num(filt.r_num),_c_num(filt.c_num),_angles(nullptr),_old_grad(nullptr),grad_proc(nullptr)
{
    _angles = new double*[filt.r_num];
    for(int i = 0; i < filt.r_num; ++i)
    {
        _angles[i] = new double[filt.c_num];
    }
    _old_grad = new double*[filt.r_num];
    for(int i = 0; i < filt.r_num; ++i)
    {
        _old_grad[i] = new double[filt.c_num];
    }
    grad_proc = new double*[filt.r_num];
    for(int i = 0; i < filt.r_num; ++i)
    {
        grad_proc[i] = new double[filt.c_num];
    }
    
    for (int i=0; i<filt.r_num; ++i)
        for ( int j=0; j<filt.c_num; ++j)
        {
            _angles[i][j]=filt.teta[i][j];
            _old_grad[i][j]=filt.g[i][j];
        }
}


Not_max_suppression::~Not_max_suppression()
{
    for(unsigned int i = 0; i < sizeof(_angles); ++i) delete _angles[i];
    delete []_angles;
    for(unsigned int i = 0; i < sizeof(_old_grad); ++i) delete _old_grad[i];
    delete []_old_grad;
    for(unsigned int i = 0; i < sizeof(grad_proc); ++i) delete grad_proc[i];
    delete []grad_proc;
}

void Not_max_suppression::fill_grad_border()
{
    for ( int i=0, n=1; i>=0; --i, n+=2)
        for(int j=1; j<_c_num+1 ; ++j)
            _old_grad[i][j]=_old_grad[i+n][j];
    for (int i=_r_num+1, n=1; i<_r_num+2; ++i, n+=2)
        for ( int j= 1; j<_c_num+1; ++j)
            _old_grad[i][j]=_old_grad[i-n][j];
    for ( int j=0, n=1; j>=0; --j,n+=2 )
        for(int i=0; i< _r_num+2; ++i )
            _old_grad[i][j]=_old_grad[i][j+n];
    for (int j=_c_num+1, n=1; j<_c_num+2; ++j,n+=2 )
        for(int i=0; i< _r_num+2; ++i )
            _old_grad[i][j]=_old_grad[i][j-n];
}


void Not_max_suppression::gradient_proc()
{
    //fill_grad_border();
    for (int i=0; i<_r_num; ++i)
        for ( int j=0; j<_c_num; ++j)
        {
            if ((i==0)||(j==0)||(i==_r_num-1)||(j==_c_num-1))
            {
                grad_proc[i][j]=_old_grad[i][j];
                continue;
            }
            else{
            if (((_angles[i][j]>=-M_PI/8)&&(_angles[i][j]<=M_PI/8))||(_angles[i][j]>=7*M_PI/8)||(_angles[i][j]<=-7*M_PI/8))
            {
                if ((_old_grad[i][j]<=_old_grad[i][j-1])||(_old_grad[i][j]<=_old_grad[i][j+1])) grad_proc[i][j]=0;
                else grad_proc[i][j]=_old_grad[i][j];
            }
            else if (((_angles[i][j]<=3*M_PI/8)&&(_angles[i][j]>M_PI/8))||((_angles[i][j]<=-5*M_PI/8)&&(_angles[i][j]>-7*M_PI/8)))
            {
                if ((_old_grad[i][j]<=_old_grad[i-1][j+1])||(_old_grad[i][j]<=_old_grad[i+1][j-1])) grad_proc[i][j]=0;
                else grad_proc[i][j]=_old_grad[i][j];
            }
            else if (((_angles[i][j]>=5*M_PI/8)&&(_angles[i][j]<7*M_PI/8))||((_angles[i][j]<-M_PI/8)&&(_angles[i][j]>=-3*M_PI/8)))
            {
                if ((_old_grad[i][j]<=_old_grad[i-1][j-1])||(_old_grad[i][j]<=_old_grad[i+1][j+1])) grad_proc[i][j]=0;
                else grad_proc[i][j]=_old_grad[i][j];
            }
            else
            {
                if ((_old_grad[i][j]<=_old_grad[i-1][j])||(_old_grad[i][j]<=_old_grad[i+1][j])) grad_proc[i][j]=0;
                else grad_proc[i][j]=_old_grad[i][j];
            }
            }
        }
}
