#include "TinyRetail_Julius.h"

/*****************************************
@ function
@ parameter
@ return
******************************************/
TinyRetail_Julius::TinyRetail_Julius(){
    fprintf(stdout,"Welcome to julius\n");
}

/*****************************************
@ function
    juliusで用いたインスタンスを開放する
@ parameter
@ return
******************************************/
TinyRetail_Julius::~TinyRetail_Julius(){
    /* calling j_close_stream(recog) at any time will terminate
        recognition and exit j_recognize_stream() */
    j_close_stream(recog); //ストリームを閉じる

    j_recog_free(recog); // インスタンスを開放する

    fprintf(stdout,"See you julius\n");
}

/*****************************************
@ function
    juliusを使うにあたっての初期設定
@ parameter
@ return
    正常終了 : 0, 異常終了 : -1
******************************************/
int TinyRetail_Julius::Begin(){
    Jconf *jconf;   //設定パラメータ格納エリア
    int ret;

    // ログが要らなきゃコメントアウト消して
    //jlog_set_output(NULL);

    /*Juliusの設定ファイルである *.jconfファイルを指定する*/
    // char* jconf_filename[]= {"../kaimono.jconf"};

    //指定した*.jconfファイルから設定を読み込む
    jconf = j_config_load_file_new("../kaimono.jconf");
    if (jconf == NULL) {		/* error */
        fprintf(stderr, "Try `-help' for more information.\n");
        return -1;
    }

    //読み込んだ*.jconfから認識器を作成する
    recog = j_create_instance_from_jconf(jconf);
    if (recog == NULL) {
        fprintf(stderr, "Error in startup\n");
        return -1;
    }

    //各種コールバックの作成
    callback_add(recog, CALLBACK_EVENT_SPEECH_READY, status_recready, NULL);
    callback_add(recog, CALLBACK_EVENT_SPEECH_START, status_recstart, NULL);
    callback_add(recog, CALLBACK_RESULT, output_result, NULL);

    // マイクなどのオーディオ周りのイニシャライズ
    if (j_adin_init(recog) == FALSE) {    /* error */
        return -1;
    }

    //エンジンの全設定と全システム情報をログに出力する.
    #ifdef detail_log 
    j_recog_info(recog);
    #endif

    //入力デバイスがちゃんと開けているか確認
    switch(j_open_stream(recog, NULL)) {
    case 0:			/* succeeded */
        break;
    case -1:      		/* error */
        fprintf(stderr, "error in input stream\n");
        return 0;
    case -2:			/* end of recognition process */
        fprintf(stderr, "failed to begin input stream\n");
        return 0;
    }
}

/*****************************************
@ function
    juliusの認識をスタートさせる
@ parameter
@ return
    正常終了 : 0, 異常終了 : -1
******************************************/
int TinyRetail_Julius::start_stream(){

    // Juliusの認識部を別スレッドで開始する
    pJulius_Thread = new Julius_Thread(recog);
    bool ret = pJulius_Thread->Begin();

    return ((ret==true) ? 0 : -1);
}

/*****************************************
@ function
    juliusの認識をストップさせる
@ parameter
@ return
******************************************/
void TinyRetail_Julius::stop_stream(){

    // 別スレッドで動作しているJulius認識部を終了させる
    j_request_terminate(recog);

    // Juliusの認識部が終了するまで待つ
    while( pJulius_Thread->fIamZombie==false ); 

    // スレッドの終了
    pJulius_Thread->End();
    delete(pJulius_Thread);
    pJulius_Thread=NULL;
}

/*****************************************
@ function
    喋れるようになったら呼び出されるコールバック
@ parameter
@ return
******************************************/
void TinyRetail_Julius::status_recready(Recog *recog, void *dummy){
    if (recog->jconf->input.speech_input == SP_MIC || recog->jconf->input.speech_input == SP_NETAUDIO) {
        fprintf(stderr, "<<< please speak >>>");
    }
}

/*****************************************
@ function
    音声入力が鳥がされた時に呼び出されるコールバック
@ parameter
@ return
******************************************/
void TinyRetail_Julius::status_recstart(Recog *recog, void *dummy){
    if (recog->jconf->input.speech_input == SP_MIC || recog->jconf->input.speech_input == SP_NETAUDIO) {
        fprintf(stderr, "\r                    \r");
    }
}

/*****************************************
@ function
    音素列を出力するサブ関数
@ parameter
@ return
******************************************/
void TinyRetail_Julius::put_hypo_phoneme(WORD_ID *seq, int n, WORD_INFO *winfo){
    int i,j;
    WORD_ID w;
    static char buf[MAX_HMMNAME_LEN];

    if (seq != NULL) {
        for (i=0;i<n;i++) {
            if (i > 0) printf(" |");
            w = seq[i];
            for (j=0;j<winfo->wlen[w];j++) {
                center_name(winfo->wseq[w][j]->name, buf);
                printf(" %s", buf);
            }
        }
    }
    printf("\n");
}

/*****************************************
@ function
    juliusの認証結果を表示させる
@ parameter
@ return
******************************************/
void TinyRetail_Julius::output_result(Recog *recog, void *dummy){

    RecogProcess *r;

    for(r=recog->process_list;r;r=r->next){

        //処理が生きていない時は飛ばす
        if(!r->live) continue;

        //結果は r->result に格納されている
        //詳細はrecog.hを参考にしてください

        //処理結果が得られない時も飛ばす
    #ifndef detail_log
        if(r->result.status < 0) continue;
    #else
              /* outout message according to the status code */
        switch(r->result.status) {
        case J_RESULT_STATUS_REJECT_POWER:
            printf("<input rejected by power>\n");
            break;
        case J_RESULT_STATUS_TERMINATE:
            printf("<input teminated by request>\n");
            break;
        case J_RESULT_STATUS_ONLY_SILENCE:
            printf("<input rejected by decoder (silence input result)>\n");
            break;
        case J_RESULT_STATUS_REJECT_GMM:
            printf("<input rejected by GMM>\n");
            break;
        case J_RESULT_STATUS_REJECT_SHORT:
            printf("<input rejected by short input>\n");
            break;
        case J_RESULT_STATUS_REJECT_LONG:
            printf("<input rejected by long input>\n");
            break;
        case J_RESULT_STATUS_FAIL:
            printf("<search failed>\n");
            break;
        }
        /* continue to next process instance */
            continue;
    #endif

        //全てのセンテンスをアウトプットする
        WORD_INFO *winfo = r->lm->winfo;

        for(int n=0; n < r->result.sentnum; n++){   //センテンスの数だけループする

            Sentence *s = &(r->result.sent[n]);     //センテンスの構造体のアドレス入手
            WORD_ID *seq = s->word;                 //ワード(文を品詞レベルまで分解したもの)の集まりのIDを取得
            int seqnum = s->word_num;               //ワードの数

            //結果の出力
            printf("sentence%d:", n+1);
            for(int i=0;i<seqnum;i++) printf(" %s", winfo->woutput[seq[i]]); //ワードの数だけ回す
            printf("\n");

            //音素列
            printf("phseq%d:", n+1);
            put_hypo_phoneme(seq, seqnum, winfo);
            printf("\n");

            //信頼度
            printf("cmscore%d:", n+1);
            for (int i=0;i<seqnum; i++) printf(" %5.3f", s->confidence[i]);
            printf("\n");

            //スコア
            printf("score%d: %f", n+1, s->score);
        }

    }
    fflush(stdout); //出力バッファをフラッシュする
}