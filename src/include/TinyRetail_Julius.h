#pragma once

/* include top Julius library header */
#include <julius/juliuslib.h>

/* Juliusの認識部を別スレッドにするためのスレッドクラス */
#include <Julius_Thread.h>

#include <stdio.h>

class Julius_Thread;
class TinyRetail_Julius{

    //※詳細なログが必要なときは定義すること
    // #define detail_log

public:
    TinyRetail_Julius();
    ~TinyRetail_Julius();
    int     Begin();
    int     start_stream();
    void    stop_stream();

private:
    static  void    put_hypo_phoneme(WORD_ID *seq, int n, WORD_INFO *winfo);
    static  void    status_recready(Recog *recog, void *dummy);
    static  void    status_recstart(Recog *recog, void *dummy);
    static  void    output_result(Recog *recog, void *dummy);

public:

private:
    Recog *recog;   //認識コアのインスタンス変数
    Julius_Thread   *pJulius_Thread;

};