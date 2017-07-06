#include <TinyRetail_Julius.h>
// #include <Post_curl.h>
#include <stdio.h>

int main(int argc,char *argv[]){

    TinyRetail_Julius *pTinyRetail = new TinyRetail_Julius();

    int ret = pTinyRetail->Begin();
    fprintf(stdout,"Begin() ret = %d\n",ret);

    getchar();

    ret = pTinyRetail->start_stream();
    fprintf(stdout,"start_stream() ret = %d\n",ret);

    getchar();

    pTinyRetail->stop_stream();
    fprintf(stdout,"stop_stream() \n");

    //終了
    delete(pTinyRetail);
    pTinyRetail=NULL;

    // Post_curl *pPost_curl = new Post_curl();

    // const char *sent_to_url="http://192.168.0.6/tinyRetail_index/get_post.php";

    // int ret = pPost_curl->Begin(sent_to_url);
    // fprintf(stdout,"Begin() ret = %d\n",ret);

    // getchar();

    // const char *post_data1="name=daniel&project=curl";
    // ret = pPost_curl->send_post(post_data1);
    // fprintf(stdout,"send_post() ret = %d\n",ret);

    // getchar();

    // const char *post_data2="name=yasuda&project=julius";
    // ret = pPost_curl->send_post(post_data2);
    // fprintf(stdout,"send_post() ret = %d\n",ret);

    return 0;
}