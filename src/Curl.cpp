#include <Curl.hpp>

CCurl::CCurl(){
    cout << "CCurl init" << endl;
}

CCurl::~CCurl(){
    cout << "CCurl see you" << endl;

    // curl_easy_init()で取得したハンドルを開放する
    // 呼び出し後のハンドルの使用は違反となる
    curl_easy_cleanup(this->m_pCurl);
    
}

/**
 * libcurlの初期設定,
 * ハンドルの取得,
 * @param to_url POSTの送信先url
 * @return true : 正常終了, false : 異常終了
 */
bool CCurl::Begin(string to_url){    

    // 受け取ったurl情報を保存
    this->m_url = to_url;

    // ハンドルを取得する
    // ハンドルは再利用することでパフォーマンス向上させる 
    this->m_pCurl = curl_easy_init();

    if (this->m_pCurl == NULL) {
            cerr << "curl_easy_init() failed" << endl;
            return false;
    }

    // 送信先URLの設定
    curl_easy_setopt(this->m_pCurl, CURLOPT_URL, to_url.c_str());
    // レスポンスデータを扱う関数を設定する(コールバック関数の設定)
    curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEFUNCTION, callbackWrite);
    // レスポンスデータの格納先
    curl_easy_setopt(this->m_pCurl, CURLOPT_WRITEDATA, &this->m_resData);

    return true;

}

/**
 * GETデータを送信する
 * @param data getで送信するデータ
 * @return レスポンスデータ
 */
string CCurl::send_get(string data){

    CURLcode ret;
    string make_url;

    // バッファのクリア
    this->m_resData = "";

    // urlにdataを足し合わせる
    make_url = this->m_url + "?" + data;

    // urlをセットする
    curl_easy_setopt(this->m_pCurl, CURLOPT_URL, make_url.c_str());

    // GET処理の実行
    ret = curl_easy_perform(this->m_pCurl);

    if (ret != CURLE_OK) {
        cerr << "curl_easy_perform() failed." << endl;
        return NULL;
    }

    return this->m_resData; 
}
/**
 * POSTデータを送信する
 * @param data postで送信するデータ
 * @return レスポンスデータ
 */
string CCurl::send_post(string data){
    
    CURLcode res;

    // バッファのクリア
    this->m_resData = "";

    //POSTデータを指定する
    curl_easy_setopt(this->m_pCurl, CURLOPT_POSTFIELDS, data.c_str());

    //POST要求の実行
    res = curl_easy_perform(this->m_pCurl);

    if(res != CURLE_OK){
        cerr << "curl_easy_perform() failed." << endl;
        return NULL;
    }

    return this->m_resData;     
}

/**
 * httpレスポンスデータのコールバック関数
 * (いまいち何をこの中でやっているかは不明)
 * @param ???
 * @return データサイズ
 */
size_t CCurl::callbackWrite(char *ptr, size_t size, size_t nmemb, string *stream){

    int dataLength = size * nmemb;
    stream->append(ptr, dataLength);

    return dataLength;
}

string CCurl::urlEncode(string data){

    char *resData;

     // dataをURLエンコードする
    resData = curl_easy_escape(this->m_pCurl, data.c_str(), data.length());
    if ( resData==NULL ){
        cerr << "can not conversion to escape" << endl;
        return NULL;
    }

    return (string)resData;
}