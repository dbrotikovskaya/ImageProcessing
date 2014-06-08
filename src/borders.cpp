#include "borders.h"

Borders::Borders(int low,int high,int r_num,int c_num,double** old_grad):
areas(),/*eq_to_one(),eq_to_1(),*/border_map_mod(nullptr),border_map(nullptr),low_bord(low),high_bord(high),_r_num(r_num),_c_num(c_num)
{
    
    border_map = new int*[r_num];
    border_map_mod = new int*[r_num];
    for(int i = 0; i < r_num; ++i)
    {
        border_map[i] = new int[c_num];
    }
    for(int i = 0; i < r_num; ++i)
    {
        border_map_mod[i] = new int[c_num];
    }
    for(int i = 0; i < _r_num; ++i)
        for(int j = 0; j < _c_num; ++j)
            if (old_grad[i][j]<=low_bord) border_map[i][j]=0;
            else if (old_grad[i][j]>=high_bord) border_map[i][j]=1;
            else  border_map[i][j]=2;
}


Borders::~Borders()
{
    for(unsigned int i = 0; i < sizeof(border_map); ++i) delete border_map[i];
    delete []border_map;
}


void Borders::find_areas()
{
    int a,b,c,d;
    int area_num=3;
    areas.push_back(set<int>());
    areas.push_back(set<int>());
    areas.push_back(set<int>());
    for(int i = 0; i < _r_num; ++i)
        for(int j = 0; j < _c_num; ++j)
        {
            if (border_map[i][j]==0) { continue; }
            if (i==0)
            {
                b=0;
                c=0;
                d=0;
                if (j==0) a=0; else a=border_map[i][j-1];
            }
            else if (j==0) {a=0; b=0;  c=border_map[i-1][j]; d=border_map[i-1][j+1];}
            else if (j==_c_num-1) { a=border_map[i][j-1]; b=border_map[i-1][j-1]; c=border_map[i-1][j]; d=0; }
            else
            {
                a=border_map[i][j-1];
                b=border_map[i-1][j-1];
                c=border_map[i-1][j];
                d=border_map[i-1][j+1];
            }
            if (border_map[i][j]==2)
            {
                if ((a==0)&&(b==0)&&(c==0)&&(d==0))
                {
                    areas.push_back(set<int>());
                    border_map[i][j]=area_num;
                    area_num++;
                }
                else
                {
                    if (a!=0)
                    {
                        if (b!=0) { areas[a].insert(b); }
                        if (c!=0) { areas[a].insert(c); }
                        if (d!=0) { areas[a].insert(d); }
                        border_map[i][j]=a;
                    }
                    if (b!=0)
                    {
                        if (a!=0) { areas[b].insert(a); }
                        if (c!=0) { areas[b].insert(c); }
                        if (d!=0) { areas[b].insert(d); }
                        border_map[i][j]=b;
                    }
                    if (c!=0)
                    {
                        if (b!=0) { areas[c].insert(b); }
                        if (a!=0) { areas[c].insert(a); }
                        if (d!=0) { areas[c].insert(d); }
                        border_map[i][j]=c;
                    }
                    if (d!=0)
                    {
                        if (b!=0) { areas[d].insert(b); }
                        if (c!=0) { areas[d].insert(c); }
                        if (a!=0) { areas[d].insert(a); }
                        border_map[i][j]=d;
                    }
                }
            }
            else if (border_map[i][j]==1)
            {
                if (a!=0) {  areas[1].insert(a); areas[a].insert(1); }
                if (b!=0) {  areas[1].insert(b); areas[b].insert(1); }
                if (c!=0) {  areas[1].insert(c); areas[c].insert(1); }
                if (d!=0) {  areas[1].insert(d); areas[d].insert(1); }
            }
            
        }
}
/*
void Borders::find_eq_to_1(int search_set)
{
    set<int>::iterator  it_s1=areas[1].begin();
    set<int>::iterator  it_ss=areas[search_set].begin();
    
    for(it_ss=areas[search_set].begin(); it_ss!=areas[search_set].end(); ++it_ss)
    {
        int flag=0;
        for(it_s1=areas[1].begin(); it_s1!=areas[1].end(); ++it_s1)
        {
            if ((*it_ss)==(*it_s1)) flag=1;
        }
        eq_to_one.insert(*it_ss);
        if (flag==0) { find_eq_to_1(*it_ss);}
    }
}*/


void Borders::find_eq_to_1_fast_new(bool* eqs)
{
    queue<int> q;
    set<int>::iterator itl;
    q.push(1);
    eqs[1] = true;
    int j;
    while ( !q.empty() ) {
        j = q.front();
        q.pop();
        for ( itl = areas[j].begin(); itl != areas[j].end(); ++itl )
        {
            uint to = *(itl);
            if ( !eqs[to] )
            {
                eqs[to] = true;
                q.push(to);
            }
        }
    }
    
}

/*
void Borders::find_eq_to_1_fast()
{
    queue<int> q;
    vector<int> viewed(areas.size(),0);
    for (unsigned int i=0; i<areas.size(); ++i) { eq_to_1.push_back(0); }
    q.push(1);
    
    while (!q.empty())
    {
        int f=q.front();
        q.pop();
        
        for ( auto &n: areas[f] ) {
            eq_to_1[n]=1;
            if (!viewed[n])
            {
                viewed[n]=1;
                q.push(n);
            }
        }
    }
    
}



void Borders::change_new_map()
{
    set<int>::iterator it=areas[1].begin();
    
    for(int i = 0; i < _r_num; ++i)
        for(int j = 0; j < _c_num; ++j)
        {
            if (border_map[i][j]==1 || border_map[i][j]==0 ) border_map_mod[i][j]=255*border_map[i][j];
            else
            {
                int flag=0;
                for(it=areas[1].begin(); it!=areas[1].end(); ++it) if (border_map[i][j]==(*it)) flag=1;
                if (flag==1) border_map_mod[i][j]=255; else border_map_mod[i][j]=0;
            }
            
        }
}


void Borders::change_new_map_fast()
{
    for(int i = 0; i < _r_num; ++i)
        for(int j = 0; j < _c_num; ++j)
        {
            if (eq_to_1[border_map[i][j]]==1) border_map_mod[i][j]=255;
            else border_map_mod[i][j]=0;
        }
}*/

void Borders::change_new_map_fast_new(bool* eqs)
{
    for(int i = 0; i < _r_num; ++i)
        for(int j = 0; j < _c_num; ++j)
        {
            if (eqs[border_map[i][j]]==true) border_map_mod[i][j]=255;
            else border_map_mod[i][j]=0;
        }
}


void Borders::hysteresis()
{
    find_areas();
    bool* eq_s = new bool[areas.size()];
    for( uint i = 0; i < areas.size(); ++i )
        eq_s[i] = false;
    find_eq_to_1_fast_new(eq_s);
    change_new_map_fast_new(eq_s);
}

bool Borders::norm_state(int i,int j)
{
    if (border_map[i][j]==0)
    {
        if (i>j)
            if (i==_r_num-1) return false;
            else return norm_state(i+1,j);
            else if(i<j)
                if (j==_c_num-1) return false;
                else return norm_state(i,j+1);
                else
                {
                    if (j==_c_num-1) return false;
                    else
                    {
                        bool b1,b2,b3;
                        b1=norm_state(i+1,j); b2=norm_state(i+1,j+1); b3=norm_state(i,j+1);
                        return (b1||b2||b3);
                    }
                }
    }
    else if (border_map[i][j]==255)
    {
        if (i>j)
            if (i==_r_num-1) return true;
            else { act_state(i+1,j); return true;}
            else if(i<j)
                if (j==_c_num-1) return true;
                else { norm_state(i,j+1); return true;}
                else
                {
                    if (j==_c_num-1) return true;
                    else
                    {
                        act_state(i+1,j); act_state(i+1,j+1); act_state(i,j+1);
                        return true;
                    }
                }
    }
    else
    {
        if (i>j)
            if (i==_r_num-1) { border_map[i][j]=0; return false;}
            else
            {
                if (norm_state(i+1,j)) { border_map[i][j]=255; return true; }
                else { border_map[i][j]=0; return false; }
            }
            else if(i<j)
                if (j==_c_num-1) { border_map[i][j]=0; return false;}
                else
                {
                    if (norm_state(i,j+1)) { border_map[i][j]=255; return true; }
                    else { border_map[i][j]=0; return false; }
                }
                else
                {
                    if (j==_c_num-1) { border_map[i][j]=0; return false;}
                    else
                    {
                        bool b1,b2,b3;
                        b1=norm_state(i+1,j); b2=norm_state(i+1,j+1); b3=norm_state(i,j+1);
                        if (b1||b2||b3) { border_map[i][j]=255; return true; }
                        else { border_map[i][j]=0; return false; }
                    }
                }
    }
}



bool Borders::act_state(int i,int j)
{
    if (border_map[i][j]==0)
    {
        if (i>j)
            if (i==_r_num-1) return false;
            else return norm_state(i+1,j);
            else if(i<j)
                if (j==_c_num-1) return false;
                else return norm_state(i,j+1);
                else
                {
                    if (j==_c_num-1) return false;
                    else
                    {
                        bool b1,b2,b3;
                        b1=norm_state(i+1,j); b2=norm_state(i+1,j+1); b3=norm_state(i,j+1);
                        return (b1||b2||b3);
                    }
                }
    }
    else if (border_map[i][j]==255)
    {
        if (i>j)
            if (i==_r_num-1) return true;
            else { act_state(i+1,j); return true;}
            else if(i<j)
                if (j==_c_num-1) return true;
                else { act_state(i,j+1); return true;}
                else
                {
                    if (j==_c_num-1) return true;
                    else
                    {
                        act_state(i+1,j); act_state(i+1,j+1); act_state(i,j+1);
                        return true;
                    }
                }
    }
    else
    {
        if (i>j)
            if (i==_r_num-1) { border_map[i][j]=255; return false;}
            else
            {
                border_map[i][j]=255; return act_state(i+1,j);
            }
            else if(i<j)
                if (j==_c_num-1) { border_map[i][j]=255; return false;}
                else
                {
                    border_map[i][j]=255; return act_state(i,j+1);
                }
                else
                {
                    if (j==_c_num-1) { border_map[i][j]=255; return false;}
                    else
                    {
                        bool b1,b2,b3;
                        b1=act_state(i+1,j); b2=act_state(i+1,j+1); b3=act_state(i,j+1);
                        return (b1||b2||b3);
                    }
                }
    }
}
