// #pragma once
#ifndef __Curl_H_
#define __Curl_H_

#include <string> 
#include <iostream> 
#include <curl/curl.h> 

using namespace std;

class CCurl{
    
public:
    CCurl();
    ~CCurl();
    bool Begin(string to_url);
    string send_get(string data); 
    string send_post(string data);
    string urlEncode(string data);
private:
    static size_t callbackWrite(char *ptr, size_t size, size_t nmemb, string *stream);

    CURL *m_pCurl;
    string m_resData;
    string m_url;

};

#endif