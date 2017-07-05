#include <TinyRetail_Julius.h>
#include <stdio.h>

int main(int argc,char *argv[]){

    TinyRetail_Julius *pTinyRetail = new TinyRetail_Julius();

    int ret = pTinyRetail->Begin();
    fprintf(stdout,"Begin() ret = %d",ret);

    getchar();

    ret = pTinyRetail->start_stream();
    fprintf(stdout,"start_stream() ret = %d",ret);

    return 0;
}