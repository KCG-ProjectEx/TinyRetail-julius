#include<fstream>
#include<iostream>
#include<string>
#include<sstream>               //文字ストリーム
#include <unordered_map>       // ヘッダファイルインクルード

#include <split.h>

using namespace std;

class CnegaPosi{

public:
    CnegaPosi(const char *str);
    ~CnegaPosi();

    void    get();
    double  cmp(vector<string> str);    

private:
    ifstream ifs;
    unordered_map<string, double> mp; 

    void    set();
};
