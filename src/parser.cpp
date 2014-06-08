#include "parser.h"

Parser::~Parser()
{
    for(int i = 0; i < _n_rows; ++i) delete kernel[i];
    delete []kernel;
}


string Parser::str_without_bl(string s)
{
    string s1="";
    unsigned int i;
    int j;
    for (i=0; (s[i]==' ')&&(i<s.length()); ++i);
    for (j=s.length()-1; (s[j]==' ')&&(j>=0) ; --j);
    for( int k=i; k<=j; ++k) s1+=s[k];
    
    return s1;
}


void Parser::get_numbers(char* str)
{
    double n;
    char c=' ';
    int cur_chr=0;
    int cur_rows=0,cur_cols=0;
    int flag=0;
    while (str[cur_chr]!='\0')
    {
        string s="";
        while((str[cur_chr]!=',')&&(str[cur_chr]!=';')&&(str[cur_chr]!='\0'))
        {
            s+=str[cur_chr];
            ++cur_chr;
        }
        
        string n_s=str_without_bl(s);
        if (n_s=="")
        {
            _n_cols=0;
            _n_rows=0;
            throw string("Empty element");
        }
        if (sscanf(n_s.c_str(),"%lg%c",&n,&c)== 1)
        {
            if (c!=' ')
            {
                _n_cols=0;
                _n_rows=0;
                throw string("Wrong elements");
            }
            _num.push_back(n);
        }
        else
        {
            _n_cols=0;
            _n_rows=0;
            throw string("Wrong number of elements");
        }
        if (str[cur_chr]==',')
        {
            ++cur_cols;
        }
        else if (str[cur_chr]==';')
        {
            ++cur_rows;
            ++cur_cols;
            if (flag==0) { _n_cols=cur_cols; cur_cols=0; flag=1; }
            else if (cur_cols!=_n_cols)
            {
                _n_cols=0;
                _n_rows=0;
                throw string("Wrong number of elements in column");
            }
            else
            {
                cur_cols=0;
            }
        }
        else
        {
            ++cur_rows;
            ++cur_cols;
            _n_rows=cur_rows;
            if (flag==0) { _n_cols=cur_cols;}
            else if (cur_cols!=_n_cols)
            {
                _n_cols=0;
                _n_rows=0;
                throw string("Wrong number of elements in column");
            }
            return;
        }
        ++cur_chr;
    }
}

void Parser::_make_space()
{
    kernel = new double*[_n_rows];
    for(int i = 0; i < _n_rows; ++i)
    {
        kernel[i] = new double[_n_cols];
    }
}


void Parser::fill_matrix()
{
    if ((_n_rows%2==0)||(_n_cols%2==0))
    {
        _n_cols=0;
        _n_rows=0;
        throw string("Wrong number of elements");
    }
    _make_space();
    for (int i=0; i<_n_rows; ++i)
    {    for ( int j=0; j<_n_cols; ++j)
        {
            kernel[i][j]=_num[i*_n_cols+j];
            //cout<<kernel[i][j]<<" ";
        }
        //cout<<endl;
    }
}

