#include <TinyRetail_Julius.h>

list<Tag_julius_result> CTinyRetail_Julius::list_result;
/*****************************************
@ function
@ parameter
@ return
******************************************/
CTinyRetail_Julius::CTinyRetail_Julius(){

    fprintf(stdout,"Welcome to julius\n");

}

/*****************************************
@ function
@ parameter
@ return
******************************************/
CTinyRetail_Julius::~CTinyRetail_Julius(){

    fprintf(stdout,"See you julius\n");
}

/*****************************************
@ function
    juliusを使うにあたっての初期設定
@ parameter
@ return
    正常終了 : 0, 異常終了 : -1
******************************************/
int CTinyRetail_Julius::Begin(int argc,char *argv[]){
    int ret;

#ifndef detail_log
    jlog_set_output(NULL);
#endif

    /*Juliusの設定ファイルである *.jconfファイルを指定する*/
    this->p_jconf = j_config_load_args_new(argc, argv);     


    if (this->p_jconf == NULL) {		/* error */
        fprintf(stderr, "Try `-help' for more information.\n");
        return -1;
    }

    //読み込んだ*.jconfから認識器を作成する
    this->p_recog = j_create_instance_from_jconf(this->p_jconf);
    if (this->p_recog == NULL) {
        fprintf(stderr, "Error in startup\n");
        return -1;
    }

    //各種コールバックの作成
    callback_add(this->p_recog, CALLBACK_EVENT_SPEECH_READY, status_recready, NULL);
    callback_add(this->p_recog, CALLBACK_EVENT_SPEECH_START, status_recstart, NULL);
    callback_add(this->p_recog, CALLBACK_RESULT, output_result, NULL);

    // マイクなどのオーディオ周りのイニシャライズ
    if (j_adin_init(this->p_recog) == FALSE) {    /* error */
        return -1;
    }

    //エンジンの全設定と全システム情報をログに出力する.
    #ifdef detail_log 
    j_recog_info(this->p_recog);
    #endif

    //入力デバイスがちゃんと開けているか確認
    switch(j_open_stream(this->p_recog, NULL)) {
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
int CTinyRetail_Julius::start_stream(){

    

    // Juliusの認識部を別スレッドで開始する
    pCJulius_Thread = new CJulius_Thread(this->p_recog);
    bool ret = pCJulius_Thread->Begin();

    return ((ret==true) ? 0 : -1);
}

/*****************************************
@ function
    juliusの認識をストップさせる
@ parameter
@ return
******************************************/
void CTinyRetail_Julius::stop_stream(){



    fprintf(stdout,"j_close_stream()\n");

    // 別スレッドで動作しているJulius認識部を終了させる/
    int ret = j_close_stream(this->p_recog);
    printf("j_close_stream ret = %d\n",ret);

    fprintf(stdout,"fIamZombie==false\n");

    // スレッド停止までまつ
    while(1){
        if( pCJulius_Thread->fIamZombie==false )break;
    }

    fprintf(stdout,"pCJulius_Thread->End()\n");
    // スレッドの終了
    pCJulius_Thread->End();
    delete(pCJulius_Thread);
    pCJulius_Thread=NULL;

    if(this->p_jconf){

        //これで開放すると、j_recog_freeで落ちる...
        //j_conf_free(this->p_jconf)
        this->p_jconf = NULL;
    }

    j_recog_free(this->p_recog); // インスタンスを開放する
    this->p_recog = NULL;
}

/*****************************************
@ function
    喋れるようになったら呼び出されるコールバック
@ parameter
@ return
******************************************/
void CTinyRetail_Julius::status_recready(Recog *recog, void *dummy){
    if (recog->jconf->input.speech_input == SP_MIC || recog->jconf->input.speech_input == SP_NETAUDIO) {
        fprintf(stderr, "<<< please speak >>>");
    }
}

/*****************************************
@ function
    音声入力がトリガーされた時に呼び出されるコールバック
@ parameter
@ return
******************************************/
void CTinyRetail_Julius::status_recstart(Recog *recog, void *dummy){
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
void CTinyRetail_Julius::put_hypo_phoneme(WORD_ID *seq, int n, WORD_INFO *winfo){
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
    juliusの認証結果を整理する
@ parameter
@ return
******************************************/
void CTinyRetail_Julius::output_result(Recog *recog, void *dummy){

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
            continue;
    #endif

        //全てのセンテンスをアウトプットする
        WORD_INFO *winfo = r->lm->winfo;

        for(int n=0; n < r->result.sentnum; n++){   //センテンスの数だけループする

            Sentence *s = &(r->result.sent[n]);     //センテンスの構造体のアドレス入手
            WORD_ID *seq = s->word;                 //ワード(文を品詞レベルまで分解したもの)の集まりのIDを取得
            int seqnum = s->word_num;               //ワードの数

            Tag_julius_result   t_r;                
            ostringstream  oss;                 // ワードをつなぎ合わせる
                                                // <iostream>のインスタンス変数

        #if 0
            printf("------------------------------\n");
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
            printf("\n");
        #else

            //結果を構造体に格納する===================================
            for(int i=0; i<seqnum; i++){
                t_r.sentence.push_back(winfo->woutput[seq[i]]);
            }

            //ossをクリアする
            oss.str("");
            oss.clear(stringstream::goodbit);

            //信頼度を構造体に格納する==================================
            for (int i=0;i<seqnum; i++){
                oss << (int)(1000 * s->confidence[i]) << flush;
            }
            t_r.confidence = oss.str();

            //ossをクリアする
            oss.str("");
            oss.clear(stringstream::goodbit);

            //スコアを構造体に格納する===================================
            t_r.score = s->score;

            push_result_data(&t_r); //結果が格納された構造体をリスト構造へ

        #endif

        }

    }
    fflush(stdout); //出力バッファをフラッシュする
}

/********************************************************
@ function
    引数の構造体を最後尾に追加する
@ parameter
    Tag_julius_result[in]   :   juliusの認証結果を格納する構造体
@ return
*********************************************************/
void CTinyRetail_Julius::push_result_data(Tag_julius_result *p_tag_tmp){
    
    //リスト内の最後尾に値を追加する
    list_result.push_back(*p_tag_tmp);

}

/********************************************************
@ function
    リスト構造から値を取り出し、戻り値でその値を返す。
@ parameter
    Tag_julius_result[out]   :   juliusの認証結果を格納する構造体
@ return
*********************************************************/
int CTinyRetail_Julius::pop_result_data(Tag_julius_result *p_tag_tmp){

    if( list_result.empty() ) return -1;  //リストが空なら

    // リストにデータがあったら==================================
    list<Tag_julius_result>::iterator itr = list_result.begin();

    p_tag_tmp->sentence = itr->sentence;
    p_tag_tmp->confidence = itr->confidence;
    p_tag_tmp->score = itr->score;

    //リスト内の先頭から要素を削除する
    list_result.pop_front();

    return 0;

}
