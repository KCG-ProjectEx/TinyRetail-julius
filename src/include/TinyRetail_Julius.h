#ifndef _TinyRetail_Julius__
#define _TinyRetail_Julius__

/* include top Julius library header */
#include <julius/juliuslib.h>

#include <stdio.h>

/*Juliusの設定ファイルである *.jconfファイルを指定する*/
#define jconf_filename "../kaimono.jconf"

class TinyRetail_Julius{

    //※詳細なログが必要なときは定義すること
    // #define detail_log

public:
    TinyRetail_Julius();
    ~TinyRetail_Julius();
    int     Begin();
    int     start_stream();

private:
    void    status_recready(Recog *recog, void *dummy);
    void    status_recstart(Recog *recog, void *dummy);
    void    put_hypo_phoneme(WORD_ID *seq, int n, WORD_INFO *winfo);
    void    output_result(Recog *recog, void *dummy);

public:

private:
    Recog *recog;   //認識コアのインスタンス変数
}

#endif //_TinyRetail_Julius__