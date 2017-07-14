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
    int ret;

    // ログが要らなきゃコメントアウト消して
    //jlog_set_output(NULL);

    /*Juliusの設定ファイルである *.jconfファイルを指定する*/
    const char* argv[]= {
            "TinyRetail",
            "-C",
            "../kaimono.jconf"
    };
    int argc = sizeof(argv)/sizeof(argv[0]);

    //指定した*.jconfファイルから設定を読み込む
    //juliusはC関数だから、const外して char**にするしかない
    // j_config_load_file_new()は警告が出てきて使えなかった
    this->p_jconf = j_config_load_args_new(argc, (char**)argv); 


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

    //認証結果を格納するリストの初期化
    // firstnode_list_WI = (List_WORD_INFO*)malloc(sizeof(List_WORD_INFO) );
    // if( firstnode_list_WI == NULL ){ //領域が確保できなかったら
	// 	fprintf(stderr,"メモリアロケートエラー\n");
    //     return -1;
	// }
    // firstnode_list_WI->winfo =  NULL;
    // firstnode_list_WI->next  =  NULL;

    // lastnode_list_WI = firstnode_list_WI;
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
    pJulius_Thread = new Julius_Thread(this->p_recog);
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



    fprintf(stdout,"j_close_stream()\n");

    // 別スレッドで動作しているJulius認識部を終了させる/
    int ret = j_close_stream(this->p_recog);
    printf("j_close_stream ret = %d\n",ret);

    fprintf(stdout,"fIamZombie==false\n");

    // スレッド停止までまつ
    while(1){
        if( pJulius_Thread->fIamZombie==false )break;
    }

    fprintf(stdout,"pJulius_Thread->End()\n");
    // スレッドの終了
    pJulius_Thread->End();
    delete(pJulius_Thread);
    pJulius_Thread=NULL;

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

            printf("------------------------------\n");
            //結果の出力
            printf("sentence%d:", n+1);
            for(int i=0;i<seqnum;i++) printf(" %s", winfo->woutput[seq[i]]); //ワードの数だけ回す
            printf("\n");

            /* ======================= */
            // CString_join *sj = new CString_join();
            // for(int i=0;i<seqnum;i++){
            //     sj->join_back(winfo->woutput[seq[i]]);
            // }
            // push_result_data(sj->req_string());
            // delete(sj);
            /* ======================= */  
            std::string str;
            for(int i=0;i<seqnum;i++){
                std::string t_str(winfo->woutput[seq[i]]);
                str += t_str;
            }
            printf("string = %s\n",str.c_str());
            /* ======================= */              

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
        }

    }
    fflush(stdout); //出力バッファをフラッシュする
}

// bool TinyRetail_Julius::push_result_data(WORD_INFO *winfo){


// }

// bool TinyRetail_Julius::pop_result_data(WORD_INFO *winfo){


// }
