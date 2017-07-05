/**
 * @file   julius-simple.cpp
 *
 * <EN>
 * @brief  Sample code for JuliusLib
 *
 * This is a simple code to link JuliusLib and do recognition.
 *
 * This program will output recognition result to stdout according
 * to the given jconf setting.
 * </EN>
 *
 * <JA>
 * @brief  JuliusLib用サンプルコード
 *
 * このコードは JuliusLib をリンクして簡単な認識器を構築します．
 *
 * このプログラムは音声入力に対して与えられた jconf 設定のもとで
 * 認識を行い，結果を標準出力に出力します．
 *
 * </JA>
 *
 * @author Akinobu Lee
 * @date   Tue Dec 11 14:40:04 2007
 *
 * $Revision: 1.5 $
 *
 */

/* include top Julius library header */
#include <julius/juliuslib.h>

/**
 * Callback to be called when start waiting speech input.
 * 喋れるようになったら呼び出されるコールバック
 */
static void
status_recready(Recog *recog, void *dummy)
{
  if (recog->jconf->input.speech_input == SP_MIC || recog->jconf->input.speech_input == SP_NETAUDIO) {
    fprintf(stderr, "<<< please speak >>>");
  }
}

/**
 * Callback to be called when speech input is triggered.
 * 音声入力がトリガされたときに呼び出されるコールバック
 */
static void
status_recstart(Recog *recog, void *dummy)
{
  if (recog->jconf->input.speech_input == SP_MIC || recog->jconf->input.speech_input == SP_NETAUDIO) {
    fprintf(stderr, "\r                    \r");
  }
}

/**
 * Sub function to output phoneme sequence.
 * 音素列を出力するSub関数
 */
static void
put_hypo_phoneme(WORD_ID *seq, int n, WORD_INFO *winfo)
{
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
/**
 * Callback to output final recognition result.
 * This function will be called just after recognition of an input ends
 * 最終的な認証結果をアウトプットするコールバック
 */
static void
output_result(Recog *recog, void *dummy)
{
  int i, j;
  int len;
  WORD_INFO *winfo;
  WORD_ID *seq;
  int seqnum;
  int n;
  Sentence *s;
  RecogProcess *r;
  HMM_Logical *p;
  SentenceAlign *align;

  /* all recognition results are stored at each recognition process
     instance */
  for(r=recog->process_list;r;r=r->next) {

    /* skip the process if the process is not alive */
    // 処理が生きてないときは飛ばす
    if (! r->live) continue;

    /* result are in r->result.  See recog.h for details */

    /* check result status */
    //処理結果が得られないときも飛ばす
    if (r->result.status < 0) {      /* no results obtained */
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
    }

    /* output results for all the obtained sentences */
    //全てのセンテンスをアウトプットする
    winfo = r->lm->winfo;

    for(n = 0; n < r->result.sentnum; n++) { /* for all sentences */
                                            //センテンスの数だけループ

      s = &(r->result.sent[n]); //センテンスの構造体のアドレス入手
      seq = s->word;            //ワード(文を品詞レベルまで分解したもの)の集まりのIDを取得
      seqnum = s->word_num;     //ワードの数

      /* output word sequence like Julius */
      //全てのセンテンスをアウトプットする
      printf("sentence%d:", n+1);
      for(i=0;i<seqnum;i++) printf(" %s", winfo->woutput[seq[i]]);
      printf("\n");
      /* LM entry sequence */
      printf("wseq%d:", n+1);
      for(i=0;i<seqnum;i++) printf(" %s", winfo->wname[seq[i]]);
      printf("\n");
      /* phoneme sequence */
      printf("phseq%d:", n+1);
      put_hypo_phoneme(seq, seqnum, winfo);
      printf("\n");
      /* confidence scores */
      printf("cmscore%d:", n+1);
      for (i=0;i<seqnum; i++) printf(" %5.3f", s->confidence[i]);
      printf("\n");
      /* AM and LM scores */
      printf("score%d: %f", n+1, s->score);
      if (r->lmtype == LM_PROB) { /* if this process uses N-gram */
	printf(" (AM: %f  LM: %f)", s->score_am, s->score_lm);
      }
      printf("\n");
      if (r->lmtype == LM_DFA) { /* if this process uses DFA grammar */
	/* output which grammar the hypothesis belongs to
	   when using multiple grammars */
	if (multigram_get_all_num(r->lm) > 1) {
	  printf("grammar%d: %d\n", n+1, s->gram_id);
	}
      }

      /* output alignment result if exist */
      for (align = s->align; align; align = align->next) {
	printf("=== begin forced alignment ===\n");
	switch(align->unittype) {
	case PER_WORD:
	  printf("-- word alignment --\n"); break;
	case PER_PHONEME:
	  printf("-- phoneme alignment --\n"); break;
	case PER_STATE:
	  printf("-- state alignment --\n"); break;
	}
	printf(" id: from  to    n_score    unit\n");
	printf(" ----------------------------------------\n");
	for(i=0;i<align->num;i++) {
	  printf("[%4d %4d]  %f  ", align->begin_frame[i], align->end_frame[i], align->avgscore[i]);
	  switch(align->unittype) {
	  case PER_WORD:
	    printf("%s\t[%s]\n", winfo->wname[align->w[i]], winfo->woutput[align->w[i]]);
	    break;
	  case PER_PHONEME:
	    p = align->ph[i];
	    if (p->is_pseudo) {
	      printf("{%s}\n", p->name);
	    } else if (strmatch(p->name, p->body.defined->name)) {
	      printf("%s\n", p->name);
	    } else {
	      printf("%s[%s]\n", p->name, p->body.defined->name);
	    }
	    break;
	  case PER_STATE:
	    p = align->ph[i];
	    if (p->is_pseudo) {
	      printf("{%s}", p->name);
	    } else if (strmatch(p->name, p->body.defined->name)) {
	      printf("%s", p->name);
	    } else {
	      printf("%s[%s]", p->name, p->body.defined->name);
	    }
	    if (r->am->hmminfo->multipath) {
	      if (align->is_iwsp[i]) {
		printf(" #%d (sp)\n", align->loc[i]);
	      } else {
		printf(" #%d\n", align->loc[i]);
	      }
	    } else {
	      printf(" #%d\n", align->loc[i]);
	    }
	    break;
	  }
	}

	printf("re-computed AM score: %f\n", align->allscore);

	printf("=== end forced alignment ===\n");
      }
    }
  }

  /* flush output buffer */
  fflush(stdout);
}


/**
 * Main function
 *
 */
int
main(int argc, char *argv[])
{
  /**
   * configuration parameter holder
   * 設定パラメータ格納エリア
   */
  Jconf *jconf;

  /**
   * Recognition instance
   * 認識コアのインスタンス変数
   */
  Recog *recog;

  /**
   * speech file name for MFCC file input
   * MFCCファイル入力用の音声ファイル名
   */
  static char speechfilename[MAXPATHLEN];

  int ret;

  /* by default, all messages will be output to standard out */
  /* to disable output, uncomment below */
  // ログが要らなきゃコメントアウト消して
  //jlog_set_output(NULL);

  /* output log to a file */
  // ログをファイルに出力したかったらコメントアウト消して
  //FILE *fp; fp = fopen("log.txt", "w"); jlog_set_output(fp);

  /* if no argument, output usage and exit */
  if (argc == 1) {
    fprintf(stderr, "Julius rev.%s - based on ", JULIUS_VERSION);
    j_put_version(stderr);
    fprintf(stderr, "Try '-setting' for built-in engine configuration.\n");
    fprintf(stderr, "Try '-help' for run time options.\n");
    return -1;
  }

  /************/
  /* Start up */
  /************/
  /* 1. load configurations from command arguments */
  // 設定した jconfファイルから設定を読み込む
  // コマンド引数からパラメータを読み込み，その値を格納した 新たな設定インスタンスを割り付けて返す. 
  jconf = j_config_load_args_new(argc, argv);
  /* else, you can load configurations from a jconf file */
  // その他のファイルから入力する場合は以下を編集
  //jconf = j_config_load_file_new(jconf_filename);

  if (jconf == NULL) {		/* error */
    fprintf(stderr, "Try `-help' for more information.\n");
    return -1;
  }

  /* 2. create recognition instance according to the jconf */
  /* it loads models, setup final parameters, build lexicon
     and set up work area for recognition */
  /*設定したjconfから認証器を作成する
   A/D-in の初期化以外で認識を開始するのに必要な処理をすべて行う*/
  recog = j_create_instance_from_jconf(jconf);
  if (recog == NULL) {
    fprintf(stderr, "Error in startup\n");
    return -1;
  }

  /*********************/
  /* Register callback */
  /*********************/
  /* register result callback functions */
  // 各種コールバックの作成
  callback_add(recog, CALLBACK_EVENT_SPEECH_READY, status_recready, NULL);
  callback_add(recog, CALLBACK_EVENT_SPEECH_START, status_recstart, NULL);
  callback_add(recog, CALLBACK_RESULT, output_result, NULL);

  /**************************/
  /* Initialize audio input */
  /**************************/
  /* initialize audio input device */
  /* ad-in thread starts at this time for microphone */
  // マイクなどのオーディオ周りのイニシャライズ
  if (j_adin_init(recog) == FALSE) {    /* error */
    return -1;
  }

  /* output system information to log */
  //エンジンの全設定と全システム情報をログに出力する. 
  j_recog_info(recog);

  /***********************************/
  /* Open input stream and recognize */
  /* 入力ストリームを開き、認識する     */
  /***********************************/

  if (jconf->input.speech_input == SP_MFCFILE || jconf->input.speech_input == SP_OUTPROBFILE) {
    /* MFCC file input */

    while (get_line_from_stdin(speechfilename, MAXPATHLEN, "enter MFCC filename->") != NULL) {
      if (verbose_flag) printf("\ninput MFCC file: %s\n", speechfilename);
      /* open the input file */
      ret = j_open_stream(recog, speechfilename);
      switch(ret) {
      case 0:			/* succeeded */
	break;
      case -1:      		/* error */
	/* go on to the next input */
	continue;
      case -2:			/* end of recognition */
	return 0;
      }
      /* recognition loop */
      ret = j_recognize_stream(recog);
      if (ret == -1) return -1;	/* error */
      /* reach here when an input ends */
      // マイク以外で何かがエラーが起きた
    }

  } else {
    /* raw speech input (microphone etc.) */
    // 入力デバイスがちゃんと開けているか確認
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

    /**********************/
    /* Recognization Loop */
    /**********************/
    /* enter main loop to recognize the input stream */
    /* finish after whole input has been processed and input reaches end */
    // 入力から認証を行うループに入る
    ret = j_recognize_stream(recog);
    if (ret == -1) return -1;	/* error(内部でエラーが起こった) */

    /*******/
    /* End */
    /*******/
  }

  /* calling j_close_stream(recog) at any time will terminate
     recognition and exit j_recognize_stream() */
  j_close_stream(recog); //ストリームを閉じる

  j_recog_free(recog); // インスタンスを開放する

  /* exit program */
  return(0);
}
