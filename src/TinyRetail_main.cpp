#include <TinyRetail_Julius.h>
#include <Post_curl.h>
#include <stdio.h>

int main(int argc,char *argv[]){

    int ret;

    //==========julius初期化==========
    TinyRetail_Julius *pTinyRetail = new TinyRetail_Julius();
    ret = pTinyRetail->Begin();
    fprintf(stdout,"julius Begin() ret = %d\n",ret);

    //==========curl初期化===========
    Post_curl *pPost_curl = new Post_curl();
    const char *sent_to_url="http://192.168.0.9/tinyRetail_index/get_post2.php";
    ret = pPost_curl->Begin(sent_to_url);
    fprintf(stdout,"curl Begin() ret = %d\n",ret);


    getchar();

    // juliusの開始(別スレッドで動く)
    ret = pTinyRetail->start_stream();

    // juliusの認識結果があれば、post(curlを使って)で送信する
    Tag_julius_result tag_tmp;
    while(1){

        // juliusに認識データが無ければ、以下は飛ばす
        if(pTinyRetail->pop_result_data(&tag_tmp) == -1) continue;

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
        if((pPost_curl->send_post(create_json.c_str())) == -1){ //異常終了したら
            fprintf(stderr,"ERR : not send to post\n");
            break;
        }
    }

    pTinyRetail->stop_stream();

    //終了
    delete(pTinyRetail);
    delete(pPost_curl);
    pTinyRetail=NULL;

    return 0;
}