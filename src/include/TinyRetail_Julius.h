#pragma once

/* include top Julius library header */
#include <julius/juliuslib.h>

/* Juliusの認識部を別スレッドにするためのスレッドクラス */
#include <Julius_Thread.h>

/* include to standard library　*/
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <list>

using namespace std;

typedef struct tag_julius_result{
    string   sentence;   //認証結果
    string   confidence; //信頼度
    double   score;      //スコア
}Tag_julius_result;

class CJulius_Thread;
class CTinyRetail_Julius{

    //※詳細なログが必要なときは定義すること
    // #define detail_log

public:
    CTinyRetail_Julius();
    ~CTinyRetail_Julius();
    int     Begin(int argc,char *argv[]);
    int     start_stream();
    void    stop_stream();
    int     pop_result_data(Tag_julius_result *p_tag_tmp);

private:
    static  void    put_hypo_phoneme(WORD_ID *seq, int n, WORD_INFO *winfo);
    static  void    status_recready(Recog *recog, void *dummy);
    static  void    status_recstart(Recog *recog, void *dummy);
    static  void    output_result(Recog *recog, void *dummy);
    static  void    push_result_data(Tag_julius_result *p_tag_tmp);


public:

private:
    Recog   *p_recog;   //認識コアのインスタンス変数
    Jconf   *p_jconf;   //juliusの設定情報を格納する
    CJulius_Thread   *pCJulius_Thread;    //スレッドを使うためののインスタンス変数

    // リストのインスタンス(認証結果を一時的に格納する)
    static  list<Tag_julius_result> list_result;
};