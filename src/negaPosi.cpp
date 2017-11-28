#include<negaPosi.h>

CnegaPosi::CnegaPosi(const char *path){

    this->ifs.open((char *)path, ifstream::in);

    if(!ifs.is_open()){
        cout<<"File Open Err [negaPosi.cpp]";
        exit(1);
    }

    // 処理のスタート
    this->set();
}

CnegaPosi::~CnegaPosi(){

    this->ifs.close();

}

double CnegaPosi::cmp(vector<string> str){

    double dRet = 0;

    for(auto itr = str.begin(); itr != str.end(); ++itr) {
        // cout << ": " << *itr << endl;

        auto z = mp.find(*itr);        // 引数の文字が設定されているか？
        if( z != mp.end() ) {
            // 設定されている場合の処理

            dRet += mp[*itr];

            cout << " " << mp[*itr] << flush;
        } else {
            // 設定されていない場合の処理

            cout << " " << "No" << flush;
        }
    }
    cout << endl;

    return dRet;
}

void CnegaPosi::set(){

    string str;

    while(getline(this->ifs,str)){ 
        vector<std::string> result = split(str, ':');    // 入力データを':'で分離
        this->mp[result[0]] = stod(result[3]);            // 分離データを漢字(要素0)をkeyとして、数値(要素3)をvalueとする
    }

}

void CnegaPosi::get(){
    for(auto itr = this->mp.begin(); itr != this->mp.end(); ++itr) {
        cout << "key = " << itr->first           // キーを表示
                        << ", val = " << itr->second << "\n";    // 値を表示
    }
}