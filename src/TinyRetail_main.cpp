#include <TinyRetail_Julius.h>
#include <Curl.hpp>
#include <JSON.h>
// #include <negaPosi.h>

#include <stdio.h>
#include <iostream>
#include <string>
#include <json11.hpp>

using namespace json11;
using std::string;

int main(int argc,char *argv[]){

    int ret;

    //==========julius初期化==========
    CTinyRetail_Julius *pCTinyRetail = new CTinyRetail_Julius();
    ret = pCTinyRetail->Begin(argc, argv);
    fprintf(stdout,"julius Begin() ret = %d\n",ret);

    //==========curl初期化===========
    CCurl *pCCurl = new CCurl();
    // const char *sent_to_url="http://tinyretail/dbconnect.php?iam=julius";    
    const char *sent_to_url="http://192.168.11.110/dbconnect.php?iam=julius";
    ret = pCCurl->Begin(sent_to_url);
    fprintf(stdout,"curl Begin() ret = %d\n",ret);

    // //==========negaPosi初期化==========
    CCurl *pCCurlNegaPoji = new CCurl();
    ret = pCCurlNegaPoji->Begin("https://api.apitore.com/api/11/sentiment/predict");
    fprintf(stdout,"curl negapoji Begin() ret = %d\n",ret);    
    // tokenを追記
    std::string reqDef = "access_token=c239e412-dafa-41b2-a2f8-e273abbc934f";
    // 識別データの追記
    reqDef += "&text=";
    


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

        // cout << tag_tmp.sentence << endl;

        // pCnegaPosi->cmp(tag_tmp.sentence);

        // // 構造体の出力(debug)
        // for(auto itr = tag_tmp.sentence.begin(); itr != tag_tmp.sentence.end(); ++itr) {
        //     cout << ": " << *itr << endl;
        // }

        // /////////////////////////////////////////////////
        // sscanf(tag_tmp.confidence.c_str(), "%d", &word_rbd);
        // printf("word_rbd = %d\n",word_rbd);
        // // if(word_rbd<900) continue;
        // //////////////////////////////////////////////////
        // 
        // sscanf(tag_tmp.sentence.c_str(),"%d%s",&word_id,word_str);
        // fprintf(stdout,"word_str = %s\n",word_str);

/***************************** */

        string a;
        a = reqDef;

        a += pCCurlNegaPoji->urlEncode(tag_tmp.sentence);

        a = pCCurlNegaPoji->send_get(a);

        std::cout << "a " << a  << std::endl;

        string err;
        const auto json = Json::parse(a, err);

        if( json.is_null() ) fprintf(stderr, "json11に送られてきたデータが空です\n");

        // std::cout << "log " << json["predict"]["sentiment"].string_value() << "\n";
        // std::cout << "log " << json["predict"]["score"].string_value() << "\n";
        // std::cerr << "err " << err << std::endl;

/***************************** */

        // 受け取ったデータをjson形式に変換
        CJSON *pCJSON = new CJSON();
        pCJSON->push("mic_id","1");
        pCJSON->push("sentence",tag_tmp.sentence);
        pCJSON->push("favor",json["predict"]["sentiment"]);
        pCJSON->push("favor_score", json["predict"]["score"]);
        // pCJSON->push("word_id",word_id+53);
        // pCJSON->push("word_rbd",word_rbd);

        // // postで送るデータを表示する
        std::cout << "send-post-JSON" << pCJSON->pop() << std::endl;

        #if 0
        // postで送信する
        if((pCCurl->send_post(pCJSON->pop())) == ""){ //異常終了したら
            fprintf(stderr,"ERR : not send to post\n");
            break;
        }
        #endif

        delete(pCJSON);        

        
    }

    pCTinyRetail->stop_stream();

    //終了
    delete(pCTinyRetail);
    delete(pCCurl);
    delete(pCCurlNegaPoji);
    pCTinyRetail=NULL;

    return 0;
}
