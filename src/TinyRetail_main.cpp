#include <TinyRetail_Julius.h>
#include <Post_curl.h>
#include <stdio.h>
#include <string>
#include <JSON.h>

using namespace std;

int main(int argc,char *argv[]){

    int ret;

    //==========julius初期化==========
    CTinyRetail_Julius *pCTinyRetail = new CTinyRetail_Julius();
    ret = pCTinyRetail->Begin(argc, argv);
    fprintf(stdout,"julius Begin() ret = %d\n",ret);

    //==========curl初期化===========
    CPost_curl *pCPost_curl = new CPost_curl();
    //const char *sent_to_url="http://10.43.0.6/dbconnect.php?iam=julius";    
    const char *sent_to_url="http://192.168.11.110/dbconnect.php?iam=julius";    
    ret = pCPost_curl->Begin(sent_to_url);
    fprintf(stdout,"curl Begin() ret = %d\n",ret);


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

        /////////////////////////////////////////////////
        sscanf(tag_tmp.confidence.c_str(), "%d", &word_rbd);
        printf("word_rbd = %d\n",word_rbd);
        if(word_rbd<900) continue;
        //////////////////////////////////////////////////
        
        sscanf(tag_tmp.sentence.c_str(),"%d%s",&word_id,word_str);
        fprintf(stdout,"word_str = %s\n",word_str);

        //////////////////////////////////////////////////
        // ================================================
        stringstream  create_json;

        // 右のようなJSONを作成する　{"word_id":"送るデータ/"}
        create_json << "{\"mic_id\":\"1\",";
        create_json << "\"word_id\":\"";
        create_json << word_id+53;
        create_json << "\",\"word_rbd\":\"";
        create_json << word_rbd;
        create_json << "\"}";
        // ================================================

        string send_json = create_json.str();

        // postで送るデータを表示する
        cout << "send-post" << send_json.c_str() << endl;

        CJSON *pCJSON = new CJSON();
        pCJSON->push("mic_id","1");
        pCJSON->push("word_id",word_id+53);
        pCJSON->push("word_rbd",word_rbd);

        cout << "send-post-JSON" << pCJSON->pop() << endl;

        delete(pCJSON);

        #if 0
        // postで送信する
        if((pCPost_curl->send_post(send_json.c_str())) == -1){ //異常終了したら
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
