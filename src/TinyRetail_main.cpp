#include <TinyRetail_Julius.h>
#include <Post_curl.h>
#include <JSON.h>
#include <negaPosi.h>

#include <stdio.h>
#include <string>

using namespace std;

int main(int argc,char *argv[]){

    int ret;

    //==========julius初期化==========
    CTinyRetail_Julius *pCTinyRetail = new CTinyRetail_Julius();
    ret = pCTinyRetail->Begin(argc, argv);
    fprintf(stdout,"julius Begin() ret = %d\n",ret);

    //==========curl初期化===========
    CPost_curl *pCPost_curl = new CPost_curl();
    const char *sent_to_url="http://tinyretail/dbconnect.php?iam=julius";    
    ret = pCPost_curl->Begin(sent_to_url);
    fprintf(stdout,"curl Begin() ret = %d\n",ret);

    //==========negaPosi初期化==========
    CnegaPosi *pCnegaPosi = new CnegaPosi("./src/dat/pn.dic");


    //getchar();

    // juliusの開始(別スレッドで動く)
    ret = pCTinyRetail->start_stream();

    // juliusの認識結果があれば、post(curlを使って)で送信する
    Tag_julius_result tag_tmp;
    ////////////////////
    int word_rbd;////////////////////
    int word_id;////////////////
    char word_str[100];/////////
    ////////////////////
    while(1){

        // juliusに認識データが無ければ、以下は飛ばす
        if(pCTinyRetail->pop_result_data(&tag_tmp) == -1) continue;

        pCnegaPosi->cmp(tag_tmp.sentence);

        // 構造体の出力(debug)
        for(auto itr = tag_tmp.sentence.begin(); itr != tag_tmp.sentence.end(); ++itr) {
            cout << ": " << *itr << endl;
        }

        // /////////////////////////////////////////////////
        // sscanf(tag_tmp.confidence.c_str(), "%d", &word_rbd);
        // printf("word_rbd = %d\n",word_rbd);
        // // if(word_rbd<900) continue;
        // //////////////////////////////////////////////////
        // 
        // sscanf(tag_tmp.sentence.c_str(),"%d%s",&word_id,word_str);
        // fprintf(stdout,"word_str = %s\n",word_str);

        // // 受け取ったデータをjson形式に変換
        // CJSON *pCJSON = new CJSON();
        // pCJSON->push("mic_id","1");
        // pCJSON->push("word_id",word_id+53);
        // pCJSON->push("word_rbd",word_rbd);

        // // postで送るデータを表示する
        // cout << "send-post-JSON" << pCJSON->pop() << endl;

        // delete(pCJSON);

        


        #if 0
        // postで送信する
        if((pCPost_curl->send_post(pCJSON->pop())) == -1){ //異常終了したら
            fprintf(stderr,"ERR : not send to post\n");
            break;
        }
        #endif

        
    }

    pCTinyRetail->stop_stream();

    //終了
    delete(pCTinyRetail);
    delete(pCPost_curl);
    pCTinyRetail=NULL;

    return 0;
}
