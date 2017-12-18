#include <TinyRetail_Julius.h>
#include <Curl.hpp>
#include <JSON.h>
// #include <negaPosi.h>

#include <stdio.h>
#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

using namespace boost::property_tree;

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

        // 受け取ったデータをnega poji APIに投げる
        std::string strReq = reqDef;
        strReq += tag_tmp.sentence;

        std::string res; 
        res = pCCurlNegaPoji->send_get(strReq);

        // string型をstring stream型に変更
        std::stringstream stmData;
        stmData << res;

        // jsonデータを得るために、ptreeに格納する
        ptree pt;
        read_json(stmData, pt);

        // jsonデータの獲得
        if (boost::optional<std::string> str = pt.get_optional<std::string>("predict.sentiment")) {
            std::cout << "sentiment : " << str.get() << std::endl;
            res = str.get();
        }
        else {
            std::cout << "sentiment is nothing" << std::endl;
        }
        if (boost::optional<double> value = pt.get_optional<double>("predict.score")) {
            std::cout << "score : " << value.get() << std::endl;
        }
        else {
            std::cout << "score is nothing" << std::endl;
        }

/***************************** */

        // 受け取ったデータをjson形式に変換
        CJSON *pCJSON = new CJSON();
        pCJSON->push("mic_id","1");
        pCJSON->push("sentence",tag_tmp.sentence);
        pCJSON->push("favor",res);
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
