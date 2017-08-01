#include <TinyRetail_Julius.h>
#include <Post_curl.h>
#include <stdio.h>

int main(int argc,char *argv[]){

    int ret;

    //==========julius初期化==========
    CTinyRetail_Julius *pCTinyRetail = new CTinyRetail_Julius();
    ret = pCTinyRetail->Begin();
    fprintf(stdout,"julius Begin() ret = %d\n",ret);

    //==========curl初期化===========
    CPost_curl *pCPost_curl = new CPost_curl();
    const char *sent_to_url="http://10.43.0.58/tinyretail_web/dbdev.php?iam=julius";    
    ret = pCPost_curl->Begin(sent_to_url);
    fprintf(stdout,"curl Begin() ret = %d\n",ret);


    getchar();

    // juliusの開始(別スレッドで動く)
    ret = pCTinyRetail->start_stream();

    // juliusの認識結果があれば、post(curlを使って)で送信する
    Tag_julius_result tag_tmp;
    ////////////////////
    int num;/////////////////////////////////////////////
    ////////////////////
    while(1){

        // juliusに認識データが無ければ、以下は飛ばす
        if(pCTinyRetail->pop_result_data(&tag_tmp) == -1) continue;

        /////////////////////////////////////////////////
        sscanf(tag_tmp.confidence.c_str(), "%d", &num);
        printf("num = %d\n",num);
        if(num<900) continue;
        //////////////////////////////////////////////////
        
        // ================================================
        string  create_json="";

        // 右のようなJSONを作成する　{"word_id":"送るデータ"}
        create_json += "{\"word_id\":\"";
        create_json += tag_tmp.sentence;
        create_json += "\"}";
        // ================================================

        // postで送るデータを表示する
        cout << "send-post" << create_json << endl;

        // postで送信する
        if((pCPost_curl->send_post(create_json.c_str())) == -1){ //異常終了したら
            fprintf(stderr,"ERR : not send to post\n");
            break;
        }
    }

    pCTinyRetail->stop_stream();

    //終了
    delete(pCTinyRetail);
    delete(pCPost_curl);
    pCTinyRetail=NULL;

    return 0;
}